# Copyright (C) 2010 The Android Open Source Project
# Copyright (C) 2016 Andrew Gunnerson <andrewgunnerson@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# This is a horrible hack to allow prebuilt shared libraries to be included
# without copying them to TARGET_OUT

LOCAL_BUILD_SCRIPT := PREBUILT_STATIC_LIBRARY
LOCAL_MODULE_CLASS := PREBUILT_STATIC_LIBRARY
LOCAL_MAKEFILE     := $(local-makefile)

LOCAL_PREBUILT_PREFIX := lib
LOCAL_PREBUILT_SUFFIX := $(TARGET_SONAME_EXTENSION)

# Prebuilt static libraries don't need to be copied to TARGET_OUT
# to facilitate debugging, so use the prebuilt version directly
# at link time.
LOCAL_BUILT_MODULE := $(call local-prebuilt-path,$(LOCAL_SRC_FILES))
LOCAL_BUILT_MODULE_NOT_COPIED := true

include $(BUILD_SYSTEM)/prebuilt-library.mk

# ;)
$(info $(LOCAL_MODULE) is using a horribly ugly ndk-build hack that will probably break in the future!)
