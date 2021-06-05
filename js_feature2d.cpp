#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include "jsbindings.hpp"
#include "js_size.hpp"
#include "js_point.hpp"
#include "js_mat.hpp"
#include "js_alloc.hpp"
#include "js_keypoint.hpp"

typedef cv::Ptr<cv::FeatureDetector> JSFeature2DData;

JSValue feature2d_proto = JS_UNDEFINED, feature2d_class = JS_UNDEFINED;
JSClassID js_feature2d_class_id;

extern "C" VISIBLE int js_feature2d_init(JSContext*, JSModuleDef*);

static JSValue
js_feature2d_ctor(JSContext* ctx, JSValueConst new_target, int argc, JSValueConst* argv) {
  JSFeature2DData feature2d, *s;
  JSValue obj = JS_UNDEFINED;
  JSValue proto;

  if(!(s = js_allocate<JSFeature2DData>(ctx)))
    return JS_EXCEPTION;
  new(s) JSFeature2DData();

  /* using new_target to get the prototype is necessary when the
     class is extended. */
  proto = JS_GetPropertyStr(ctx, new_target, "prototype");
  if(JS_IsException(proto))
    goto fail;
  obj = JS_NewObjectProtoClass(ctx, proto, js_feature2d_class_id);
  JS_FreeValue(ctx, proto);
  if(JS_IsException(obj))
    goto fail;
  JS_SetOpaque(obj, s);
  return obj;
fail:
  js_deallocate(ctx, s);
  JS_FreeValue(ctx, obj);
  return JS_EXCEPTION;
}

VISIBLE JSFeature2DData*
js_feature2d_data(JSContext* ctx, JSValueConst val) {
  return static_cast<JSFeature2DData*>(JS_GetOpaque(val, js_feature2d_class_id));
}

VISIBLE JSValue
js_feature2d_new(JSContext* ctx, const JSFeature2DData& f2d) {
  JSValue ret;
  JSFeature2DData* s;

  if(JS_IsUndefined(feature2d_proto))
    js_feature2d_init(ctx, NULL);

  ret = JS_NewObjectProtoClass(ctx, feature2d_proto, js_feature2d_class_id);

  s = js_allocate<JSFeature2DData>(ctx);

  *s = f2d;

  JS_SetOpaque(ret, s);
  return ret;
}

VISIBLE JSValue
js_feature2d_wrap(JSContext* ctx, const JSFeature2DData& f2d) {
  return js_feature2d_new(ctx, f2d);
}

static JSValue
js_feature2d_fast(JSContext* ctx, JSValueConst new_target, int argc, JSValueConst* argv) {
  cv::Ptr<cv::FastFeatureDetector> fast;

  fast = cv::FastFeatureDetector::create();

  return js_feature2d_wrap(ctx, fast);
}

static JSValue
js_feature2d_gftt(JSContext* ctx, JSValueConst new_target, int argc, JSValueConst* argv) {
  cv::Ptr<cv::GFTTDetector> gftt;

  gftt = cv::GFTTDetector::create();

  return js_feature2d_wrap(ctx, gftt);
}

static JSValue
js_feature2d_surf(JSContext* ctx, JSValueConst new_target, int argc, JSValueConst* argv) {
  cv::Ptr<cv::xfeatures2d::SURF> surf;

  surf = cv::xfeatures2d::SURF::create();

  return js_feature2d_wrap(ctx, surf);
}

static JSValue
js_feature2d_kaze(JSContext* ctx, JSValueConst new_target, int argc, JSValueConst* argv) {
  cv::Ptr<cv::KAZE> kaze;

  kaze = cv::KAZE::create();

  return js_feature2d_wrap(ctx, kaze);
}

static JSValue
js_feature2d_freak(JSContext* ctx, JSValueConst new_target, int argc, JSValueConst* argv) {
  cv::Ptr<cv::xfeatures2d::FREAK> freak;

  freak = cv::xfeatures2d::FREAK::create();

  return js_feature2d_wrap(ctx, freak);
}

void
js_feature2d_finalizer(JSRuntime* rt, JSValue val) {
  JSFeature2DData* s = static_cast<JSFeature2DData*>(JS_GetOpaque(val, js_feature2d_class_id));
  /* Note: 's' can be NULL in case JS_SetOpaque() was not called */

  s->~JSFeature2DData();
  js_deallocate(rt, s);
}

static JSValue
js_feature2d_inspect(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv) {
  JSFeature2DData* s = js_feature2d_data(ctx, this_val);
  JSValue obj = JS_NewObjectClass(ctx, js_feature2d_class_id);

  JS_DefinePropertyValueStr(ctx, obj, "empty", JS_NewBool(ctx, (*s)->empty()), JS_PROP_ENUMERABLE);
  JS_DefinePropertyValueStr(ctx, obj, "defaultName", JS_NewString(ctx, (*s)->getDefaultName().c_str()), JS_PROP_ENUMERABLE);
  return obj;
}

enum { METHOD_DETECT = 0 };

static JSValue
js_feature2d_method(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv, int magic) {
  JSFeature2DData* s = static_cast<JSFeature2DData*>(JS_GetOpaque2(ctx, this_val, js_feature2d_class_id));
  JSValue ret = JS_UNDEFINED;

  switch(magic) {}
  return ret;
}

enum { PROP_EMPTY = 0, PROP_DEFAULT_NAME };

static JSValue
js_feature2d_getter(JSContext* ctx, JSValueConst this_val, int magic) {
  JSFeature2DData* s = static_cast<JSFeature2DData*>(JS_GetOpaque2(ctx, this_val, js_feature2d_class_id));
  JSValue ret = JS_UNDEFINED;

  switch(magic) {
    case PROP_DEFAULT_NAME: {
      std::string name = (*s)->getDefaultName();

      ret = JS_NewStringLen(ctx, name.data(), name.size());
      break;
    }
  }
  return ret;
}

JSClassDef js_feature2d_class = {
    .class_name = "Feature2D",
    .finalizer = js_feature2d_finalizer,
};

const JSCFunctionListEntry js_feature2d_proto_funcs[] = {
    JS_CFUNC_MAGIC_DEF("detect", 2, js_feature2d_method, METHOD_DETECT),
    JS_CGETSET_MAGIC_DEF("empty", js_feature2d_getter, 0, PROP_EMPTY),
    JS_CGETSET_MAGIC_DEF("defaultName", js_feature2d_getter, 0, PROP_DEFAULT_NAME),
    JS_PROP_STRING_DEF("[Symbol.toStringTag]", "Feature2D", JS_PROP_CONFIGURABLE),
};

extern "C" int
js_feature2d_init(JSContext* ctx, JSModuleDef* m) {

  /* create the Feature2D class */
  JS_NewClassID(&js_feature2d_class_id);
  JS_NewClass(JS_GetRuntime(ctx), js_feature2d_class_id, &js_feature2d_class);

  feature2d_proto = JS_NewObject(ctx);
  JS_SetPropertyFunctionList(ctx, feature2d_proto, js_feature2d_proto_funcs, countof(js_feature2d_proto_funcs));
  JS_SetClassProto(ctx, js_feature2d_class_id, feature2d_proto);
  js_set_inspect_method(ctx, feature2d_proto, js_feature2d_inspect);

  // feature2d_class = JS_NewCFunction2(ctx, js_feature2d_ctor, "Feature2D", 2, JS_CFUNC_constructor, 0);
  // JS_SetConstructor(ctx, feature2d_class, feature2d_proto);
  JSValue feature2d_fast = JS_NewCFunction2(ctx, js_feature2d_fast, "FastFeatureDetector", 0, JS_CFUNC_constructor, 0);
  JSValue feature2d_gftt = JS_NewCFunction2(ctx, js_feature2d_gftt, "GFTTDetector", 0, JS_CFUNC_constructor, 0);
  JSValue feature2d_surf = JS_NewCFunction2(ctx, js_feature2d_surf, "SURF", 0, JS_CFUNC_constructor, 0);
  JSValue feature2d_kaze = JS_NewCFunction2(ctx, js_feature2d_kaze, "KAZE", 0, JS_CFUNC_constructor, 0);
  JSValue feature2d_freak = JS_NewCFunction2(ctx, js_feature2d_freak, "FREAK", 0, JS_CFUNC_constructor, 0);

  if(m) {
    JS_SetModuleExport(ctx, m, "FastFeatureDetector", feature2d_fast);
    JS_SetModuleExport(ctx, m, "GFTTDetector", feature2d_gftt);
    JS_SetModuleExport(ctx, m, "SURF", feature2d_surf);
    JS_SetModuleExport(ctx, m, "KAZE", feature2d_kaze);
    JS_SetModuleExport(ctx, m, "FREAK", feature2d_freak);
  }

  return 0;
}

#ifdef JS_FEATURE2D_MODULE
#define JS_INIT_MODULE VISIBLE js_init_module
#else
#define JS_INIT_MODULE js_init_module_feature2d
#endif

extern "C" VISIBLE void
js_feature2d_export(JSContext* ctx, JSModuleDef* m) {
  JS_AddModuleExport(ctx, m, "FastFeatureDetector");
  JS_AddModuleExport(ctx, m, "GFTTDetector");
  JS_AddModuleExport(ctx, m, "SURF");
  JS_AddModuleExport(ctx, m, "KAZE");
  JS_AddModuleExport(ctx, m, "FREAK");
}

extern "C" JSModuleDef*
JS_INIT_MODULE(JSContext* ctx, const char* module_name) {
  JSModuleDef* m;
  m = JS_NewCModule(ctx, module_name, &js_feature2d_init);
  if(!m)
    return NULL;
  js_feature2d_export(ctx, m);
  return m;
}