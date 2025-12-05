#pragma once

#include "guns/RaylikeTypes.h"
#include "SpellStorage.h"
#include "Wand.h"

class TinkeringMenu {
private:
    SpellStorage cylinder;
    SpellStorage inventory;

public:
    SpellStorage& getCylinder() { return cylinder; }
    const SpellStorage& getCylinder() const { return cylinder; }

    SpellStorage& getInventory() { return inventory; }
    const SpellStorage& getInventory() const { return inventory; }

};



