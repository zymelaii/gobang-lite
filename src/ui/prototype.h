#ifndef UI_PROTOTYPE_H
#define UI_PROTOTYPE_H

#include <string.h>
#include <stddef.h>
#include <vector>
#include <functional>
#include <type_traits>

namespace gobang::ui {

#if defined(__GNUC__)
#define UI_OBJECT \
virtual char* __class__() const override { \
    char func[256], *p = func; \
    strcpy(func, __PRETTY_FUNCTION__); \
    p += strlen(func); \
    while (*--p != ':') {} \
    *--p = '\0'; \
    while (*--p != ':' && *p != ' ') {} \
    return strdup(p + 1); \
}
#else
#define UI_OBJECT \
virtual char* __class__() const override { \
    char func[] = __FUNCTION__, *p = func; \
    p += strlen(func); \
    while (*--p != ':') {} \
    *--p = '\0'; \
    while (*--p != ':' && *p != ' ') {} \
    return strdup(p + 1); \
}
#endif

class UiObject {
public:
    template <typename Fn> struct signal;
    template <typename RetType, typename... Args>
    struct signal<RetType(Args...)> {
        void connect(std::function<RetType(Args...)> &&slot) {
            m_slots.emplace_back(std::move(slot));
        }
        RetType operator()(Args... args) {
            m_slots.shrink_to_fit();
            for (auto &slot : m_slots) {
                slot(args...);
            }
        }
    private:
        std::vector<std::function<RetType(Args...)>> m_slots;
    };
public:
    const char* get_class() const;
protected:
    UiObject();
    UiObject(const char *name);
    void reset(const char *name);
private:
    virtual char* __class__() const = 0;
    char m_classname[64];
};

};

#endif /*UI_PROTOTYPE_H*/
