#include "main.h"

#define INITIAL_DYNAMIC_BUFFER_SIZE 128

typedef struct{
    Uint8 *data;
    size_t size; // number of bytes used
    size_t capacity; // number of bytes allocated
    Sint64 position; // current offset for read/write
}DynamicBuffer;

static size_t DynamicBufferRead(void *userdata,void *ptr,size_t len,SDL_IOStatus *status){
    DynamicBuffer *buf=userdata;
    size_t remaining=buf->size-(size_t)buf->position;
    size_t toRead=(len<remaining)?len:remaining;
    if(toRead>0){
        SDL_memcpy(ptr,buf->data+buf->position,toRead);
        buf->position+=toRead;
    }
    if(status)*status=(toRead==len)?SDL_IO_STATUS_READY:SDL_IO_STATUS_EOF;
    return toRead;
}

static size_t DynamicBufferWrite(void *userdata,const void *ptr,size_t len,SDL_IOStatus *status){
    DynamicBuffer *buf=userdata;
    Sint64 end=buf->position+(Sint64)len;
    if((size_t)end>buf->capacity){
        size_t newCap=buf->capacity;
        while(newCap<(size_t)end)newCap<<=1;
        Uint8 *newData=SDL_realloc(buf->data,newCap);
        if(!newData){
            SDL_OutOfMemory();
            if(status)*status=SDL_IO_STATUS_ERROR;
            return 0;
        }
        buf->data=newData;
        buf->capacity=newCap;
    }
    SDL_memcpy(buf->data+buf->position,ptr,len);
    buf->position+=len;
    if((size_t)buf->position>buf->size)buf->size=(size_t)buf->position;
    if(status)*status=SDL_IO_STATUS_READY;
    return len;
}

static Sint64 DynamicBufferSeek(void *userdata,Sint64 offset,SDL_IOWhence whence){
    DynamicBuffer *buf=userdata;
    Sint64 newPos=-1;
    switch(whence){
        case SDL_IO_SEEK_SET: newPos=offset; break;
        case SDL_IO_SEEK_CUR: newPos=buf->position+offset; break;
        case SDL_IO_SEEK_END: newPos=(Sint64)buf->size+offset; break;
        default: break;
    }
    if(newPos<0 || (size_t)newPos>buf->size){
        SDL_SetError("Seek out of bounds");
        return -1;
    }
    return buf->position=newPos;
}

static Sint64 DynamicBufferSize(void *userdata){
    DynamicBuffer *buf=userdata;
    return buf->size;
}

static bool DynamicBufferFlush(void *userdata,SDL_IOStatus *status){
    (void)userdata;
    if(status)*status=SDL_IO_STATUS_READY;
    return true;
}

static bool DynamicBufferClose(void *userdata){
    DynamicBuffer *buf=userdata;
    SDL_free(buf->data);
    SDL_free(buf);
    return true;
}

SDL_IOStream *NewDynamicBufferStream(Uint8 ***data){ // optionally returns a pointer to the pointer to the data buffer
    DynamicBuffer *buf=SDL_malloc(sizeof(DynamicBuffer));
    if(!buf){
        SDL_OutOfMemory();
        return NULL;
    }
    SDL_memset(buf,0,sizeof(DynamicBuffer));
    buf->capacity=INITIAL_DYNAMIC_BUFFER_SIZE;
    buf->data=SDL_malloc(buf->capacity);
    if(!buf->data){
        SDL_free(buf);
        SDL_OutOfMemory();
        return NULL;
    }
    SDL_IOStreamInterface iface;
    SDL_INIT_INTERFACE(&iface);
    iface.read=DynamicBufferRead;
    iface.write=DynamicBufferWrite;
    iface.seek=DynamicBufferSeek;
    iface.size=DynamicBufferSize;
    iface.flush=DynamicBufferFlush;
    iface.close=DynamicBufferClose;
    SDL_IOStream *stream=SDL_OpenIO(&iface,buf);
    if(!stream){
        SDL_free(buf->data);
        SDL_free(buf);
    }
    else if(data)*data=&buf->data;
    return stream;
}
