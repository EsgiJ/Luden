#pragma once

#define CLASS()
#define COMPONENT()
#define PROPERTY()

// Disable RTTR macros when reflection system is not available
#ifndef RTTR_ENABLE
#define RTTR_ENABLE(...)
#endif

#ifndef RTTR_REGISTRATION
#define RTTR_REGISTRATION
#endif

#ifndef RTTR_REGISTRATION_FRIEND
#define RTTR_REGISTRATION_FRIEND
#endif
