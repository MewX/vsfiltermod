// CSRI

#include "kcsri.h"
#include "kUtils.h"

kCSRI::kCSRI()
{
	kInstance = NULL;
}

int kCSRI::Open(std::wstring wstr)
{
	// Close
	if (kInstance) csri_close(kInstance);
	kInstance = NULL;

	// CSRI variables
	csri_rend* renderer=NULL;

	// Matching renderer not found, fallback to default
	renderer = csri_renderer_default();
	if (!renderer)
		return 1; // no CSRI

	// Open from memory
	std::vector<char> data;
	SaveMemory(data,wstr);
	if(data.size()==0)
		return 2; // no data
	kInstance = csri_open_mem(renderer,&data[0],data.size(),NULL);
	return 0;
}

void kCSRI::Close()
{
	if (kInstance) csri_close(kInstance); // if opened
	kInstance = NULL;
}

int kCSRI::Draw(videoframe& dst)
{
	// Check if CSRI loaded properly
	if (!kInstance) return 1; // no CSRI loaded

	// Load data into frame
	csri_frame frame;
	frame.pixfmt = CSRI_F_BGR_;
	for(size_t i=0;i<4;i++)
	{
		frame.planes[i] = NULL;
		frame.strides[0] = 0;
	}
	frame.planes[0] = dst.data;
	frame.strides[0] = 4*dst.width;

	// Set format
	csri_fmt format;
	format.width = dst.width;
	format.height = dst.height;
	format.pixfmt = frame.pixfmt;
	int error = csri_request_fmt(kInstance,&format);
	if (error) return 2; // incompatible format

	// Render
	csri_render(kInstance,&frame,0);
	return 0;
}