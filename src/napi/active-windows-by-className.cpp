#include "wmctrl-napi.hpp"

using namespace Napi;

class ActiveByClassNameWorker : public AsyncWorker {
    public:
        ActiveByClassNameWorker(Napi::Env &env, Promise::Deferred deferred, const char *win_class_name)
        :AsyncWorker(env), deferred(deferred), win_class_name(strdup(win_class_name)){}
        ~ActiveByClassNameWorker() {
            free(this->win_class_name);
        }

    void Execute() override {
        st = active_windows_by_class_name(wmctrljs::disp, this->win_class_name);
        wmctrljs::sync();
    }

    void OnOK() override {
        Napi::Env env = Env();
        char *err;
        String err_js;

        if (this->st != WINDOWS_ACTIVATED) {
            err = get_libwmctrl_error("activeWindowsByClassName", st);
            err_js = String::New(env, err);
            free(err);
            this->deferred.Reject(err_js);
        }
        else {
            this->deferred.Resolve(Boolean::New(env, true));
        }
    }

    private:
        Promise::Deferred deferred;
        char *win_class_name;
        enum STATES st;
};

Boolean activeWindowsByClassNameSync(const CallbackInfo &info) {
    checkClassName(info, "activeWindowsByClassNameSync");
    std::string win_class_name;
    enum STATES st;
    Env env = info.Env();

    st = active_windows_by_class_name(wmctrljs::disp, win_class_name.c_str());
    wmctrljs::sync();

    if (st != WINDOWS_ACTIVATED) {
        handling_libwmctrl_error(env, "activeWindowsByClassNameSync", st);
        return Boolean::New(env, false);
    }

    return Boolean::New(env, true);
}

Promise activeWindowsByClassNameAsync(const CallbackInfo &info) {
    checkClassName(info, "activeWindowsByClassName");
    std::string win_class_name;
    Env env = info.Env();

    win_class_name = info[0].As<String>().Utf8Value();
    Promise::Deferred deferred = Promise::Deferred::New(env);
    ActiveByClassNameWorker *wk = new ActiveByClassNameWorker(env, deferred, win_class_name.c_str());
    wk->Queue();
    return deferred.Promise();
}