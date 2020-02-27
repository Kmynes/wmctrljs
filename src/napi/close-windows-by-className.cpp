#include "wmctrl-napi.hpp"

using namespace Napi;

class CloseByClassNameWorker : public AsyncWorker {
    public:
        CloseByClassNameWorker(Napi::Env &env, Promise::Deferred deferred, const char *win_class_name)
        :AsyncWorker(env), deferred(deferred), win_class_name(strdup(win_class_name)) {}
        ~CloseByClassNameWorker() {
            free(this->win_class_name);
        }

    void Execute() override {
        st = close_windows_by_class_name(wmctrljs::disp, this->win_class_name);
        wmctrljs::sync();
    }

    void OnOK() override {
        Napi::Env env = Env();
        char *err;
        String err_js;

        if (st != WINDOWS_CLOSED) {
            err = get_libwmctrl_error("closeWindowsByClassName", st);
            err_js = String::New(env, err);
            free(err);
            this->deferred.Reject(err_js);
        }else {
            this->deferred.Resolve(Boolean::New(env, true));
        }
    }

    private:
        Promise::Deferred deferred;
        char *win_class_name;
        enum STATES st;
};

Boolean closeWindowsByClassNameSync(const CallbackInfo &info) {
    checkClassName(info, "closeWindowsByClassNameSync");
    Env env = info.Env();
    std::string win_class_name = info[0].As<String>().Utf8Value();;
    enum STATES st;
 
    st = close_windows_by_class_name(NULL, win_class_name.c_str());
    if (st != WINDOWS_ACTIVATED) {
        handling_libwmctrl_error(env, "closeWindowsByClassNameSync", st);
        return Boolean::New(env, false);
    }
    return Boolean::New(env, true);
}

Promise closeWindowsByClassNameAsync(const CallbackInfo &info) {
    checkClassName(info, "closeWindowsByClassName");
    Env env = info.Env();
    std::string win_class_name = info[0].As<String>().Utf8Value();
    Promise::Deferred deferred = Promise::Deferred::New(env);
    CloseByClassNameWorker *wk = new CloseByClassNameWorker(env, deferred, win_class_name.c_str());
    wk->Queue();
    return deferred.Promise();
}