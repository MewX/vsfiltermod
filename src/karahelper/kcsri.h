// CSRI Renderer
#include "stdafx.h"
#include <csri/csri.h>

struct videoframe
{
    int width;
    int height;
    BYTE* data;
};

class kCSRI
{
private:
    csri_inst*	kInstance;

public:
    kCSRI();

    int Open(std::wstring wstr); // text == ass file
    int Draw(videoframe& dst);
    void Close();
};