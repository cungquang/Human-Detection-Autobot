#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define VIDEO_DEVICE "/dev/video0"
#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 720

int main() {
    int fd;
    fd = open(VIDEO_DEVICE, O_RDWR);
    if (fd == -1) {
        perror("Error opening device");
        exit(EXIT_FAILURE);
    }

    struct v4l2_format fmt = {0};
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

    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    printf("Requesting buff\n");  
    if (-1 == ioctl(fd, VIDIOC_REQBUFS, &req)) {
        perror("Error requesting Buffer");
        exit(EXIT_FAILURE);
    }

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    printf("Querying buff\n");   
    if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buf)) {
        perror("Error querying Buffer");
        exit(EXIT_FAILURE);
    }

    void* buffer_start;
    buffer_start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

    printf("Queue buff\n");   
    if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) {
        perror("Error queue Buffer");
        exit(EXIT_FAILURE);
    }

    printf("Start capture\n");   
    if (-1 == ioctl(fd, VIDIOC_STREAMON, &buf.type)) {
        perror("Error start Capture");
        exit(EXIT_FAILURE);
    }

    printf("Dequeue buff\n");
    if (-1 == ioctl(fd, VIDIOC_DQBUF, &buf)) {
        perror("Error dequeue Buffer");
        exit(EXIT_FAILURE);
    }

    // Save MJPEG data to a file
    FILE* outfile;
    outfile = fopen("image.jpg", "wb");
    //outfile = fopen("image.yuv", "wb");
    fwrite(buffer_start, buf.bytesused, 1, outfile);
    fclose(outfile);

    printf("EXPORTED THE IMAGE\n");
    munmap(buffer_start, buf.length);
    close(fd);

    return 0;
}
