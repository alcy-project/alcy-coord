#include "app/driver_config.h"

namespace app {

ValidationStatus validate_config(const DriverConfig& config) {
  using Vs = ValidationStatus;

  if (config.color_mode == term::ColorMode::Unknown) [[unlikely]] {
    return Vs::InvalidColorMode;
  }

  return Vs::Success;
}

}  // namespace app
