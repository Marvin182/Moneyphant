#include "version.h"

const mr::Version& appVersion() { return mr::Version::app(); }

const mr::Version& libVersion() { return mr::Version::lib(); }