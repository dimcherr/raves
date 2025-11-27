#include "units.h"
#include "unit/ucamera.h"
#include "unit/uinteract.h"

void registerUnits() {
    units.push_back(&ucamera);
    units.push_back(&uinteract);
}

