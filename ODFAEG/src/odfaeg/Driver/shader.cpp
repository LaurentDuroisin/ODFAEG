#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
namespace odfaeg {
    namespace driver {
        Shader::Shader() {
        }
        void Shader::compile(std::string sroucecode) {

        }
        void Shader::link() {
            int fd;
            int i, j;
            uint8_t color;
            struct drm_dev_t *dev_head, *dev;

            /* init */
            fd = drm_open(dri_path);
            dev_head = drm_find_dev(fd);

            if (dev_head == NULL) {
                fprintf(stderr, "available drm_dev not found\n");
                return EXIT_FAILURE;
            }

            printf("available connector(s)\n\n");
            for (dev = dev_head; dev != NULL; dev = dev->next) {
                printf("connector id:%d\n", dev->conn_id);
                printf("\tencoder id:%d crtc id:%d fb id:%d\n", dev->enc_id, dev->crtc_id, dev->fb_id);
                printf("\twidth:%d height:%d\n", dev->width, dev->height);
            }

            /* FIXME: use first drm_dev */
            dev = dev_head;
            drm_setup_fb(fd, dev);

            /* draw something */
            for (i = 0; i < byteCode.size(); i++) {
                *(dev->buf + i) = (uint32_t) byteCode[i];
            }
            /* destroy */
        }
        void Shader::run() {
        }
        void Shader::fatal (char* str) {
            std::cerr<<str<<std::endl;
        }
        void Shader::error (char* str) {
            std::cerr<<str<<std::endl;
        }
        int Shader::eopen(const char* path, int flag) {
            int fd;
            if (fd = open(path, flag) < 0) {
                std::cerr<<"cannot open "<<path<<std::endl;
            }
        }
        void* Shader::emap(int addr, std::size_t len, int prot, int flag, int fd, off_t offset) {
            uint32_t *fp;
            if ((fp = (uint32_t *) mmap(0, len, prot, flag, offset))== MAP_FAILED) {
                error("mmap");
            }
            return fp;
        }
        int Shader::drm_open(const char *path) {
            int fd, flags;
            uint64_t has_dump;
            fd = eopen(path, O_RDWR);
            if ((flags = fcntl(fd, F_GETFD)) < 0
                || fcntl(fd, F_SETFD, flags | FD_CLOEXEC) < 0) {
                fatal("fcntl1 FD_CLOEXEC failed");
            }
            if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || has_dumb == 0) {
                fatal("drmGetCap DRM_CAP_DUMP_BUFFER failed or doesn't have dumb buffer");
            }
            return fd;
        }
        DrmDev* Shader::drm_find_dev(int fd) {
            int i;
            DrmDev* dev = nullptr, *devHead = nullptr;
            drmModeRes *res;
            drmModeConnector *conn;
            drmModeEncoder *enc;
            if ((res = drmModeGetResources(id)) == nullptr) {
                fatal("drmModeGeResources() failed");
            }
            for (i = 0; i < res->count_connectors; i++) {
                conn = drmModeGetConnector(fd, res->connectors[i]);
                if (conn != nullptr && conn->connection == DRM_MODE_CONNECTED && conn->count_modes > 0) {
                    dev = new DrmDev();
                    memset(dev, 0, sizeof(DrmDev));
                    dev->conn_id = conn->connector_id;
                    dev->enc_id = conn->encoder_id;
                    dev->next = nullptr;
                    memcpy(&dev->mode, &conn->modes[0], sizeof(drmModeInfo));
                    dev->width = conn->modes[0].hdisplay;
                    dev->height = conn->modes[0].vdisplay;
                    /* FIXME: use default encoder/crtc pair */
                    if ((enc = drmModeGetEncoder(fd, dev->enc_id)) == nullptr) {
                        fatal("drmModeGetEncoder() failed");
                    }
                    dev->crtc_id = enc->crtc_id;
                    drmModeFreeEncoder(enc);

                    dev->saved_crtc = nullptr;

                    /* create dev list */
                    dev->next = dev_head;
                    dev_head = dev;
                }
                drmModeFreeConnector(conn);
            }
            drmModeFreeResources(res);
            return dev_head;
        }
        void Shader::drm_setup_fb(int fd, struct drm_dev_t *dev)
        {
            struct drm_mode_create_dumb creq;
            struct drm_mode_map_dumb mreq;

            memset(&creq, 0, sizeof(struct drm_mode_create_dumb));
            creq.width = dev->width;
            creq.height = dev->height;
            creq.bpp = BPP; // hard conding

            if (drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq) < 0)
                fatal("drmIoctl DRM_IOCTL_MODE_CREATE_DUMB failed");

            dev->pitch = creq.pitch;
            dev->size = creq.size;
            dev->handle = creq.handle;

            if (drmModeAddFB(fd, dev->width, dev->height,
                DEPTH, BPP, dev->pitch, dev->handle, &dev->fb_id))
                fatal("drmModeAddFB failed");

            memset(&mreq, 0, sizeof(struct drm_mode_map_dumb));
            mreq.handle = dev->handle;

            if (drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq))
                fatal("drmIoctl DRM_IOCTL_MODE_MAP_DUMB failed");
            dev->buf = (uint32_t *) emmap(0, byteCode.size(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, mreq.offset);

            dev->saved_crtc = drmModeGetCrtc(fd, dev->crtc_id); /* must store crtc data */
            if (drmModeSetCrtc(fd, dev->crtc_id, dev->fb_id, 0, 0, &dev->conn_id, 1, &dev->mode))
                fatal("drmModeSetCrtc() failed");
        }

        void drm_destroy(int fd, struct drm_dev_t *dev_head)
        {
            struct drm_dev_t *devp, *devp_tmp;
            struct drm_mode_destroy_dumb dreq;

            for (devp = dev_head; devp != NULL;) {
                if (devp->saved_crtc)
                    drmModeSetCrtc(fd, devp->saved_crtc->crtc_id, devp->saved_crtc->buffer_id,
                        devp->saved_crtc->x, devp->saved_crtc->y, &devp->conn_id, 1, &devp->saved_crtc->mode);
                drmModeFreeCrtc(devp->saved_crtc);

                munmap(devp->buf, devp->size);

                drmModeRmFB(fd, devp->fb_id);

                memset(&dreq, 0, sizeof(dreq));
                dreq.handle = devp->handle;
                drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);

                devp_tmp = devp;
                devp = devp->next;
                free(devp_tmp);
            }

            close(fd);
        }
}
