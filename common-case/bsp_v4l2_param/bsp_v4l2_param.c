#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#define V4L2_MAX_CTRL (128)
#define V4L2_MAX_FMT (128)

int main(int argc, char *argv[])
{
	int err;
	int fd;
	int i = 0;
	char *dev_path = NULL;
	int buf_mp_flag = 0;

	if(argc < 2)
	{
		printf("usage: bsp_v4l2_param [dev_path]\n");
		return -1;
	}

	dev_path = argv[1];
	fd = open(dev_path, O_RDWR);

	if (fd < 0)
    {
    	printf("usage: bsp_v4l2_param open dev failed fd: %d\n", fd);
		return -1;
	}
	
	/*capability test*/
	struct v4l2_capability v4l2_cap;
	memset(&v4l2_cap, 0, sizeof(struct v4l2_capability));
	err = ioctl(fd, VIDIOC_QUERYCAP, &v4l2_cap);
	if (err < 0)
    {
		printf("usage: bsp_v4l2_param VIDIOC_QUERYCAP failed err: %d\n", err);
		return -1;
	}
	
	printf("[%s]: v4l2_cap.driver: %s \n", dev_path, v4l2_cap.driver);
	printf("[%s]: v4l2_cap.card: %s \n", dev_path, v4l2_cap.card);
	printf("[%s]: v4l2_cap.bus_info: %s \n", dev_path, v4l2_cap.bus_info);
	printf("[%s]: v4l2_cap.version: 0x%x \n", dev_path, v4l2_cap.version);
	printf("[%s]: v4l2_cap.capabilities: 0x%x \n", dev_path, v4l2_cap.capabilities);

	if(v4l2_cap.capabilities & 
	(V4L2_CAP_VIDEO_CAPTURE_MPLANE
	| V4L2_CAP_VIDEO_OUTPUT_MPLANE
	| V4L2_CAP_VIDEO_M2M_MPLANE))
	{
		buf_mp_flag = 1;
	}


	/*frame format test*/
	struct v4l2_fmtdesc fmt_dsc;
	
	if(v4l2_cap.capabilities & 
	(V4L2_CAP_VIDEO_M2M_MPLANE
	|V4L2_CAP_VIDEO_M2M))
	{
		memset(&fmt_dsc, 0, sizeof(struct v4l2_fmtdesc));
		for(i = 0; i < V4L2_MAX_FMT; i++)
		{
			fmt_dsc.index = i;
			fmt_dsc.type = (buf_mp_flag ? 
				V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE : V4L2_BUF_TYPE_VIDEO_OUTPUT);
			err = ioctl(fd, VIDIOC_ENUM_FMT, &fmt_dsc);
		
			if (err < 0)
			{
				if(i <= 0)
				{
					printf("VIDIOC_ENUM_FMT failed err:%d\n", err);
				}
				break;
			}

			printf("---enum output format------\n");
			printf("[%s]: fmt_dsc.index: %d \n", dev_path, fmt_dsc.index);
			printf("[%s]: fmt_dsc.type: 0x%x \n", dev_path, fmt_dsc.type);
			printf("[%s]: fmt_dsc.flags: 0x%x \n", dev_path, fmt_dsc.flags);
			printf("[%s]: fmt_dsc.description: %s \n", dev_path, fmt_dsc.description);
			printf("[%s]: fmt_dsc.pixelformat: 0x%x \n", dev_path, fmt_dsc.pixelformat);
			printf("\n");
		}
	}

	memset(&fmt_dsc, 0, sizeof(struct v4l2_fmtdesc));
	for(i = 0; i < V4L2_MAX_FMT; i++)
	{
		fmt_dsc.index = i;
		fmt_dsc.type = (buf_mp_flag ? 
			V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE : V4L2_BUF_TYPE_VIDEO_CAPTURE);
		err = ioctl(fd, VIDIOC_ENUM_FMT, &fmt_dsc);
		
		if (err < 0)
		{
			if(i <= 0)
			{
				printf("VIDIOC_ENUM_FMT failed err:%d\n", err);
			}
			break;
		}

		printf("---enum capture format------\n");
		printf("[%s]: fmt_dsc.index: %d \n", dev_path, fmt_dsc.index);
		printf("[%s]: fmt_dsc.type: 0x%x \n", dev_path, fmt_dsc.type);
		printf("[%s]: fmt_dsc.flags: 0x%x \n", dev_path, fmt_dsc.flags);
		printf("[%s]: fmt_dsc.description: %s \n", dev_path, fmt_dsc.description);
		printf("[%s]: fmt_dsc.pixelformat: 0x%x \n", dev_path, fmt_dsc.pixelformat);
		printf("\n");
	}
	
	struct v4l2_format  v4l2_fmt;
	memset(&v4l2_fmt, 0, sizeof(struct v4l2_format));
	v4l2_fmt.type = (buf_mp_flag ? 
		V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE : V4L2_BUF_TYPE_VIDEO_CAPTURE);
	
	if(1 == buf_mp_flag)
	{
		v4l2_fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_YUYV;
    	v4l2_fmt.fmt.pix_mp.width       = 1280;
		v4l2_fmt.fmt.pix_mp.height      = 720;
    	v4l2_fmt.fmt.pix_mp.field       = V4L2_FIELD_NONE;
	}
	else
	{
		v4l2_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    	v4l2_fmt.fmt.pix.width       = 1280;
		v4l2_fmt.fmt.pix.height      = 720;
    	v4l2_fmt.fmt.pix.field       = V4L2_FIELD_NONE;
	}

	err = ioctl(fd, VIDIOC_S_FMT, &v4l2_fmt); 
	
    if (err) 
    {
		printf("VIDIOC_S_FMT fail err: %d\n", err);      
    }

	memset(&v4l2_fmt, 0, sizeof(struct v4l2_format));
	v4l2_fmt.type = (buf_mp_flag ? 
		V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE : V4L2_BUF_TYPE_VIDEO_CAPTURE);
	err = ioctl(fd, VIDIOC_G_FMT, &v4l2_fmt);
	
	if (err < 0)
    {
    	printf("VIDIOC_G_FMT fail err: %d\n", err);
	}

	if(1 == buf_mp_flag)
	{
		printf("\n");
		printf("[%s]: v4l2_fmt.type: %d \n", dev_path, v4l2_fmt.type);
		printf("[%s]: v4l2_fmt.fmt.pix_mp.width: %d \n", dev_path, v4l2_fmt.fmt.pix_mp.width);
		printf("[%s]: v4l2_fmt.fmt.pix_mp.height: %d \n", dev_path, v4l2_fmt.fmt.pix_mp.height);
		printf("[%s]: v4l2_fmt.fmt.pix_mp.pixelformat: 0x%x \n", dev_path, v4l2_fmt.fmt.pix_mp.pixelformat);
		printf("[%s]: v4l2_fmt.fmt.pix_mp.field: %d \n", dev_path, v4l2_fmt.fmt.pix_mp.field);
		printf("[%s]: v4l2_fmt.fmt.pix_mp.num_planes: %d \n", dev_path, v4l2_fmt.fmt.pix_mp.num_planes);
		printf("[%s]: v4l2_fmt.fmt.pix_mp.flags: 0x%x \n", dev_path, v4l2_fmt.fmt.pix_mp.flags);
		printf("[%s]: v4l2_fmt.fmt.pix_mp.colorspace: %d \n", dev_path, v4l2_fmt.fmt.pix_mp.colorspace);

	}
	else
	{
		printf("\n");
		printf("[%s]: v4l2_fmt.type: %d \n", dev_path, v4l2_fmt.type);
		printf("[%s]: v4l2_fmt.fmt.pix.width: %d \n", dev_path, v4l2_fmt.fmt.pix.width);
		printf("[%s]: v4l2_fmt.fmt.pix.height: %d \n", dev_path, v4l2_fmt.fmt.pix.height);
		printf("[%s]: v4l2_fmt.fmt.pix.pixelformat: 0x%x \n", dev_path, v4l2_fmt.fmt.pix.pixelformat);
		printf("[%s]: v4l2_fmt.fmt.pix.field: %d \n", dev_path, v4l2_fmt.fmt.pix.field);
		printf("[%s]: v4l2_fmt.fmt.pix.bytesperline: %d \n", dev_path, v4l2_fmt.fmt.pix.bytesperline);
		printf("[%s]: v4l2_fmt.fmt.pix.sizeimage: %d \n", dev_path, v4l2_fmt.fmt.pix.sizeimage);
		printf("[%s]: v4l2_fmt.fmt.pix.colorspace: %d \n", dev_path, v4l2_fmt.fmt.pix.colorspace);
		printf("\n");
	}

	struct v4l2_streamparm streamparm;

	memset(&streamparm, 0, sizeof(struct v4l2_streamparm));
	streamparm.type = (buf_mp_flag ? 
		V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE : V4L2_BUF_TYPE_VIDEO_CAPTURE);
    err = ioctl(fd, VIDIOC_G_PARM, &streamparm);
	
	if (err) 
    {
		printf("VIDIOC_G_PARM failed err: %d\n", err);       
    }

	printf("[%s]:streamparm.parm.capture.capability 0x%x \n", 
			dev_path, streamparm.parm.capture.capability);
	printf("[%s]:streamparm.parm.capture.capturemode 0x%x \n", 
			dev_path, streamparm.parm.capture.capturemode);
	printf("[%s]:streamparm.parm.capture.timeperframe.denominator: %d \n",
			dev_path, streamparm.parm.capture.timeperframe.denominator);
	printf("[%s]:streamparm.parm.capture.timeperframe.numerator: %d \n",
			dev_path, streamparm.parm.capture.timeperframe.numerator);

	struct v4l2_selection selection;

	selection.type = (buf_mp_flag ? 
		V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE : V4L2_BUF_TYPE_VIDEO_CAPTURE);
	selection.target = V4L2_SEL_TGT_COMPOSE;

	err = ioctl(fd, VIDIOC_G_SELECTION, &selection);
	
	if (err) 
    {
	    printf("VIDIOC_G_SELECTION failed err: %d\n", err);  
    }

	printf("selection.r.left: %d, selection.r.top: %d\n", 
			selection.r.left, selection.r.top);
	printf("selection.r.width: %d, selection.r.height: %d\n", 
			selection.r.width, selection.r.height);

	/* v4l2_queryctrl test */

	struct v4l2_queryctrl qctrl;
	memset(&qctrl, 0, sizeof(struct v4l2_queryctrl));
	for(i = 0; i < V4L2_MAX_CTRL; i++)
	{
		qctrl.id = V4L2_CID_BASE + i;
		err = ioctl(fd, VIDIOC_QUERYCTRL, &qctrl);
		
		if(0 == err)
		{
			printf("\n");
			printf("[%s]: qctrl.id: 0x%x \n", dev_path, qctrl.id);
			printf("[%s]: qctrl.type: %d \n", dev_path, qctrl.type);
			printf("[%s]: qctrl.name: %s \n", dev_path, qctrl.name);
			printf("[%s]: qctrl.minimum: %d \n", dev_path, qctrl.minimum);
			printf("[%s]: qctrl.maximum: %d \n", dev_path, qctrl.maximum);
			printf("[%s]: qctrl.step: %d \n", dev_path, qctrl.step);
			printf("[%s]: qctrl.default_value: %d \n", dev_path, qctrl.default_value);
			printf("[%s]: qctrl.flags: 0x%x \n", dev_path, qctrl.flags);
			printf("\n");
		}
		
	}
	
	return 0;
}




