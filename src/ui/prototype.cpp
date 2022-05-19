#include "prototype.h"

#include <string.h>
#include <malloc.h>

#define UiObjectClassPrefix "ZYMelaii.gobang-lite"

namespace gobang::ui {

UiObject::UiObject() {
	strcpy(m_classname, UiObjectClassPrefix);
}

UiObject::UiObject(const char *name) {
	reset(name);
}

void UiObject::reset(const char *name) {
	strcpy(m_classname, UiObjectClassPrefix ".");
	strcat(m_classname, name);
	free((void*)name);
}

const char* UiObject::get_class() const {
	return m_classname;
}

}