#include "BUFFERHEADERTYPE.h"
Nan::Persistent<v8::Function> BUFFERHEADERTYPE::constructor;

NAN_MODULE_INIT(BUFFERHEADERTYPE::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("BUFFERHEADERTYPE").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("nAllocLen").ToLocalChecked(), nAllocLenGet, nAllocLenSet);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("nFilledLen").ToLocalChecked(), nFilledLenGet, nFilledLenSet);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("nOffset").ToLocalChecked(), nOffsetGet, nOffsetSet);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("nFlags").ToLocalChecked(), nFlagsGet, nFlagsSet);
  Nan::SetPrototypeMethod(tpl, "set", set);
  Nan::SetPrototypeMethod(tpl, "get", get);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("BUFFERHEADERTYPE").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

BUFFERHEADERTYPE::BUFFERHEADERTYPE(OMX_BUFFERHEADERTYPE* buf) : buf(buf), first_packet(true) {
}

BUFFERHEADERTYPE::~BUFFERHEADERTYPE() {
}

NAN_METHOD(BUFFERHEADERTYPE::New) {
  if (info.IsConstructCall()) {
    OMX_BUFFERHEADERTYPE* buf = (OMX_BUFFERHEADERTYPE*) (v8::Local<v8::External>::Cast(info[0])->Value());

    BUFFERHEADERTYPE *obj = new BUFFERHEADERTYPE(buf);
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {info[0]};
    v8::Local<v8::Function> cons = Nan::New(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

NAN_GETTER(BUFFERHEADERTYPE::nAllocLenGet) {
  BUFFERHEADERTYPE* obj = Nan::ObjectWrap::Unwrap<BUFFERHEADERTYPE>(info.This());
  info.GetReturnValue().Set(obj->buf->nAllocLen);
}

NAN_SETTER(BUFFERHEADERTYPE::nAllocLenSet) {
  BUFFERHEADERTYPE* obj = Nan::ObjectWrap::Unwrap<BUFFERHEADERTYPE>(info.This());
  int nAllocLen = Nan::To<int>(value).FromJust();
  obj->buf->nAllocLen = nAllocLen;
}

NAN_GETTER(BUFFERHEADERTYPE::nFilledLenGet) {
  BUFFERHEADERTYPE* obj = Nan::ObjectWrap::Unwrap<BUFFERHEADERTYPE>(info.This());
  info.GetReturnValue().Set(obj->buf->nFilledLen);
}

NAN_SETTER(BUFFERHEADERTYPE::nFilledLenSet) {
  BUFFERHEADERTYPE* obj = Nan::ObjectWrap::Unwrap<BUFFERHEADERTYPE>(info.This());
  int nFilledLen = Nan::To<int>(value).FromJust();
  obj->buf->nFilledLen = nFilledLen;
}

NAN_GETTER(BUFFERHEADERTYPE::nOffsetGet) {
  BUFFERHEADERTYPE* obj = Nan::ObjectWrap::Unwrap<BUFFERHEADERTYPE>(info.This());
  info.GetReturnValue().Set(obj->buf->nOffset);
}

NAN_SETTER(BUFFERHEADERTYPE::nOffsetSet) {
  BUFFERHEADERTYPE* obj = Nan::ObjectWrap::Unwrap<BUFFERHEADERTYPE>(info.This());
  int nOffset = Nan::To<int>(value).FromJust();
  obj->buf->nOffset = nOffset;
}

NAN_GETTER(BUFFERHEADERTYPE::nFlagsGet) {
  BUFFERHEADERTYPE* obj = Nan::ObjectWrap::Unwrap<BUFFERHEADERTYPE>(info.This());
  info.GetReturnValue().Set(obj->buf->nFlags);
}

NAN_SETTER(BUFFERHEADERTYPE::nFlagsSet) {
  BUFFERHEADERTYPE* obj = Nan::ObjectWrap::Unwrap<BUFFERHEADERTYPE>(info.This());
  int nFlags = Nan::To<int>(value).FromJust();
  obj->buf->nFlags = nFlags;
}

// Paramerers: buffer, bool lastPacket

NAN_METHOD(BUFFERHEADERTYPE::set) {
  BUFFERHEADERTYPE* obj = Nan::ObjectWrap::Unwrap<BUFFERHEADERTYPE>(info.This());
  OMX_BUFFERHEADERTYPE* buf = obj->buf;

  v8::Local<v8::Object> bufferObj = info[0]->ToObject();
  char* bufferData = node::Buffer::Data(bufferObj);
  size_t bufferLength = node::Buffer::Length(bufferObj);

  bool lastPacket = false;
  if (!info[1]->IsUndefined()) {
    lastPacket = Nan::To<bool>(info[1]).FromJust();
  }

  if (bufferLength > buf->nAllocLen) { // bound check
    bufferLength = buf->nAllocLen;
  }

  memcpy(buf->pBuffer, bufferData, bufferLength);

  buf->nFilledLen = bufferLength;

  if (obj->first_packet) {
    buf->nFlags = OMX_BUFFERFLAG_STARTTIME;
    obj->first_packet = false;
  } else {
    buf->nFlags = OMX_BUFFERFLAG_TIME_UNKNOWN;
  }

  if (lastPacket) {
    buf->nFlags |= OMX_BUFFERFLAG_EOS;
  }

  info.GetReturnValue().Set(info.This());
}

static void freeCallback(char *data, void *hint) {
}

NAN_METHOD(BUFFERHEADERTYPE::get) {
  BUFFERHEADERTYPE* obj = Nan::ObjectWrap::Unwrap<BUFFERHEADERTYPE>(info.This());

  // Note that NewBuffer takes ownership of the pointer and will call free on it when garbage collection occurs.
  Nan::MaybeLocal<v8::Object> buffer = Nan::NewBuffer((char*) obj->buf->pBuffer, obj->buf->nFilledLen, &freeCallback, NULL);

  obj->buf->nFilledLen = 0;

  info.GetReturnValue().Set(buffer.ToLocalChecked());
}