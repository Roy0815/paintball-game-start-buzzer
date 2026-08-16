#pragma once

// Debounced reading of the RF relay signal on GPIO4 (active low).
namespace RelayControl {
    void begin();
    void update();
    bool isActive();
}
