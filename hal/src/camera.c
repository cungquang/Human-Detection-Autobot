#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "camera.h"
#include "app_helper.h"
#include "hal_helper.h"
#include <time.h>
#include <libv4l2.h>
#include "buzzer.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

// Partially learned from https://www.marcusfolkesson.se/blog/capture-a-picture-with-v4l2/
// Partially learned from https://github.com/derekmolloy/boneCV/blob/master/grabber.c

#define VIDEO_DEVICE "/dev/video0"
#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 960

struct buffer {
        void   *start;
        size_t length;
};


static void xioctl(int fh, int request, void *arg)
{
        int r;

        do {
                r = v4l2_ioctl(fh, request, arg);
        } while (r == -1 && ((errno == EINTR) || (errno == EAGAIN)));

        if (r == -1) {
                fprintf(stderr, "error %d, %s\n", errno, strerror(errno));
                exit(EXIT_FAILURE);
        }
}

static int fd;
static struct v4l2_format fmt = {0};
struct v4l2_buffer buf = {0};
struct buffer *buffers;
unsigned int n_buffers;

void configureCamera(){
    // Open camera device
    fd = v4l2_open(VIDEO_DEVICE, O_RDWR | O_NONBLOCK, 0);
    if (fd == -1) {
        perror("Error opening device");
        exit(EXIT_FAILURE);
    }

    // Set pixel format
    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = 1280;
    fmt.fmt.pix.height      = 960;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
    //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

    xioctl(fd, VIDIOC_S_FMT, &fmt);


    // Request buffers
    struct v4l2_requestbuffers req = {0};
    req.count = 2;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    //printf("Requesting buff\n");  
    xioctl(fd, VIDIOC_REQBUFS, &req);

    // buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    // buf.memory = V4L2_MEMORY_MMAP;
    // buf.index = 0;

    // // Querying buffer
    // //printf("Querying buff\n");   
    // if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buf)) {
    //     perror("Error querying Buffer");
    //     exit(EXIT_FAILURE);
    // }
    buffers = calloc(req.count, sizeof(*buffers));
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
            CLEAR(buf);

            buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory      = V4L2_MEMORY_MMAP;
            buf.index       = n_buffers;

            xioctl(fd, VIDIOC_QUERYBUF, &buf);

            buffers[n_buffers].length = buf.length;
            buffers[n_buffers].start = v4l2_mmap(NULL, buf.length,
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            fd, buf.m.offset);

            if (MAP_FAILED == buffers[n_buffers].start) {
                    perror("mmap");
                    exit(EXIT_FAILURE);
            }
    }

    for (unsigned int i = 0; i < n_buffers; ++i) {
            CLEAR(buf);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;
            xioctl(fd, VIDIOC_QBUF, &buf);
    }
}

char* captureImage() {
    // void* buffer_start;
    // buffer_start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

    // // Queue the buffer
    // printf("Queue buff\n");   
    // if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) {
    //     perror("Error queue Buffer");
    //     exit(EXIT_FAILURE);
    // }

    // // Start the capture
    // printf("Start capture\n");   
    // if (-1 == ioctl(fd, VIDIOC_STREAMON, &buf.type)) {
    //     perror("Error start Capture");
    //     exit(EXIT_FAILURE);
    // }

    // // Dequeue the buffer
    // printf("Dequeue buff\n");
    // if (-1 == ioctl(fd, VIDIOC_DQBUF, &buf)) {
    //     perror("Error dequeue Buffer");
    //     exit(EXIT_FAILURE);
    // }
    configureCamera();

    char* filename = malloc(100 * sizeof(char));
    // Save jpg data to a file
    time_t rawtime;
    struct tm *timeinfo;

    // Get the current time
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    xioctl(fd, VIDIOC_STREAMON, &buf.type);
    for (int i = 0; i < 1; i++) {
            CLEAR(buf);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            xioctl(fd, VIDIOC_DQBUF, &buf);

            FILE* outfile;

            // Format the filename with timestamp
            strftime(filename, 100, "image_%d_%H-%M-%S.ppm", timeinfo);
            outfile = fopen(filename, "w");
            if (!outfile) {
                    perror("Cannot open image");
                    exit(EXIT_FAILURE);
            }
            fprintf(outfile, "P6\n%d %d 255\n",
                    fmt.fmt.pix.width, fmt.fmt.pix.height);
            fwrite(buffers[buf.index].start, buf.bytesused, 1, outfile);
            fclose(outfile);

            xioctl(fd, VIDIOC_QBUF, &buf);
    }



    // temporarily just make it called this
    //strcpy(filename, "image1.jpg");

    xioctl(fd, VIDIOC_STREAMOFF, &buf.type);
    for (unsigned int i = 0; i < n_buffers; ++i)
            v4l2_munmap(buffers[i].start, buffers[i].length);
        v4l2_close(fd);

    char command[200];
    char* jpgFilename = malloc(100 * sizeof(char));
    strftime(jpgFilename, 100, "image_%d_%H-%M-%S.jpg", timeinfo);
    sprintf(command, "convert %s %s", filename, jpgFilename);
    runCommand(command);
    play_sound();
    printf("Exported the image as %s\n", jpgFilename);
    sprintf(command, "rm %s", filename);
    runCommand(command);
    free(filename);
    
    return jpgFilename;
}

void cameraShutdown() {
    close(fd);
}

