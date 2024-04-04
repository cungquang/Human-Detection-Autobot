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
#include <time.h>

// Partially learned from https://www.marcusfolkesson.se/blog/capture-a-picture-with-v4l2/

#define VIDEO_DEVICE "/dev/video0"
#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 720

static int fd;
static struct v4l2_format fmt = {0};
struct v4l2_buffer buf = {0};

void configureCamera(){
    // Open camera device
    fd = open(VIDEO_DEVICE, O_RDWR);
    if (fd == -1) {
        perror("Error opening device");
        exit(EXIT_FAILURE);
    }

    // Set pixel format
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = IMAGE_WIDTH;
    fmt.fmt.pix.height = IMAGE_HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (-1 == ioctl(fd, VIDIOC_S_FMT, &fmt)) {
        perror("Error setting Pixel Format");
        exit(EXIT_FAILURE);
    }

    // Request buffers
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    //printf("Requesting buff\n");  
    if (-1 == ioctl(fd, VIDIOC_REQBUFS, &req)) {
        perror("Error requesting Buffer");
        exit(EXIT_FAILURE);
    }

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    // Querying buffer
    //printf("Querying buff\n");   
    if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buf)) {
        perror("Error querying Buffer");
        exit(EXIT_FAILURE);
    }
}

char* captureImage() {
    void* buffer_start;
    buffer_start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

    // Queue the buffer
    printf("Queue buff\n");   
    if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) {
        perror("Error queue Buffer");
        exit(EXIT_FAILURE);
    }

    // Start the capture
    printf("Start capture\n");   
    if (-1 == ioctl(fd, VIDIOC_STREAMON, &buf.type)) {
        perror("Error start Capture");
        exit(EXIT_FAILURE);
    }

    // Dequeue the buffer
    printf("Dequeue buff\n");
    if (-1 == ioctl(fd, VIDIOC_DQBUF, &buf)) {
        perror("Error dequeue Buffer");
        exit(EXIT_FAILURE);
    }

    // Save jpg data to a file
    char* filename = malloc(100 * sizeof(char));
    time_t rawtime;
    struct tm *timeinfo;

    // Get the current time
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Format the filename with timestamp
    strftime(filename, 100, "image_%d_%H-%M-%S.jpg", timeinfo);

    FILE* outfile;

    // temporarily just make it called this
    //strcpy(filename, "image1.jpg");

    outfile = fopen(filename, "wb");
    //outfile = fopen("image.yuv", "wb");
    fwrite(buffer_start, buf.bytesused, 1, outfile);
    fclose(outfile);

    printf("Exported the image as %s\n", filename);
    munmap(buffer_start, buf.length);
    return filename;
}

void cameraShutdown() {
    close(fd);
}

