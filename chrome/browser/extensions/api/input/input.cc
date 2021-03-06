// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/extensions/api/input/input.h"

#include "base/command_line.h"
#include "base/lazy_instance.h"
#include "base/metrics/histogram.h"
#include "base/strings/string16.h"
#include "chrome/browser/extensions/extension_function_registry.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/user_metrics.h"
#include "ui/events/event.h"
#include "ui/keyboard/keyboard_controller.h"
#include "ui/keyboard/keyboard_switches.h"

#if defined(USE_ASH)
#include "ash/root_window_controller.h"
#include "ash/shell.h"
#include "ui/keyboard/keyboard_util.h"
#endif

namespace {

const char kNotYetImplementedError[] =
    "API is not implemented on this platform.";

}  // namespace

namespace extensions {

bool VirtualKeyboardPrivateInsertTextFunction::RunImpl() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
#if defined(USE_ASH)
  base::string16 text;
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(0, &text));

  return keyboard::InsertText(text, ash::Shell::GetPrimaryRootWindow());
#endif
  error_ = kNotYetImplementedError;
  return false;
}

bool VirtualKeyboardPrivateMoveCursorFunction::RunImpl() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
#if defined(USE_ASH)
  if (!CommandLine::ForCurrentProcess()->HasSwitch(
      keyboard::switches::kEnableSwipeSelection)) {
    return false;
  }

  int swipe_direction;
  int modifier_flags;
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(0, &swipe_direction));
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(1, &modifier_flags));

  return keyboard::MoveCursor(
      swipe_direction,
      modifier_flags,
      ash::Shell::GetPrimaryRootWindow()->GetDispatcher());
#endif
  error_ = kNotYetImplementedError;
  return false;
}

bool VirtualKeyboardPrivateSendKeyEventFunction::RunImpl() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
#if defined(USE_ASH)
  base::Value* options_value = NULL;
  base::DictionaryValue* params = NULL;
  std::string type;
  int char_value;
  int key_code;
  std::string key_name;
  int modifiers;

  EXTENSION_FUNCTION_VALIDATE(args_->Get(0, &options_value));
  EXTENSION_FUNCTION_VALIDATE(options_value->GetAsDictionary(&params));
  EXTENSION_FUNCTION_VALIDATE(params->GetString("type", &type));
  EXTENSION_FUNCTION_VALIDATE(params->GetInteger("charValue", &char_value));
  EXTENSION_FUNCTION_VALIDATE(params->GetInteger("keyCode", &key_code));
  EXTENSION_FUNCTION_VALIDATE(params->GetString("keyName", &key_name));
  EXTENSION_FUNCTION_VALIDATE(params->GetInteger("modifiers", &modifiers));

  return keyboard::SendKeyEvent(
      type,
      char_value,
      key_code,
      key_name,
      modifiers,
      ash::Shell::GetPrimaryRootWindow()->GetDispatcher());
#endif
  error_ = kNotYetImplementedError;
  return false;
}

bool VirtualKeyboardPrivateHideKeyboardFunction::RunImpl() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
#if defined(USE_ASH)
  UMA_HISTOGRAM_ENUMERATION(
      "VirtualKeyboard.KeyboardControlEvent",
      keyboard::KEYBOARD_CONTROL_HIDE_USER,
      keyboard::KEYBOARD_CONTROL_MAX);

  // Pass HIDE_REASON_MANUAL since calls to HideKeyboard as part of this API
  // would be user generated.
  ash::Shell::GetInstance()->keyboard_controller()->HideKeyboard(
      keyboard::KeyboardController::HIDE_REASON_MANUAL);

  return true;
#endif
  error_ = kNotYetImplementedError;
  return false;
}

bool VirtualKeyboardPrivateLockKeyboardFunction::RunImpl() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
#if defined(USE_ASH)
  bool lock;
  EXTENSION_FUNCTION_VALIDATE(args_->GetBoolean(0, &lock));

  ash::Shell::GetInstance()->keyboard_controller()->set_lock_keyboard(lock);

  return true;
#endif
  error_ = kNotYetImplementedError;
  return false;
}

bool VirtualKeyboardPrivateKeyboardLoadedFunction::RunImpl() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
#if defined(USE_ASH)
  keyboard::MarkKeyboardLoadFinished();

  base::UserMetricsAction("VirtualKeyboardLoaded");

  return true;
#endif
  error_ = kNotYetImplementedError;
  return false;
}

bool VirtualKeyboardPrivateGetKeyboardConfigFunction::RunImpl() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
#if defined(USE_ASH)
  base::DictionaryValue* results = new base::DictionaryValue();
  results->SetString("layout", keyboard::GetKeyboardLayout());
  results->SetBoolean("a11ymode", keyboard::GetAccessibilityKeyboardEnabled());
  SetResult(results);
  return true;
#endif
  error_ = kNotYetImplementedError;
  return false;
}

InputAPI::InputAPI(Profile* profile) {
}

InputAPI::~InputAPI() {
}

static base::LazyInstance<ProfileKeyedAPIFactory<InputAPI> >
g_factory = LAZY_INSTANCE_INITIALIZER;

// static
ProfileKeyedAPIFactory<InputAPI>* InputAPI::GetFactoryInstance() {
  return g_factory.Pointer();
}

}  // namespace extensions
