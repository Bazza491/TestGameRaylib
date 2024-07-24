//
// Created by BaileyPaior-Smith on 21/11/2022.
//

#include "raylib.h"
#include "SpellStorage.h"
#include "Wand.h"

Wand::Wand(float castDelay, float rechargeTime, int capacity, Texture2D texture) : castDelay(
        castDelay), rechargeTime(rechargeTime), capacity(capacity), texture(texture), spells(capacity){
}
Wand::Wand() {

}
float Wand::getCastDelay() const {
    return castDelay;
}
void Wand::setCastDelay(float delay) {
    castDelay = delay;
}
float Wand::getRechargeTime() const {
    return rechargeTime;
}
int Wand::getCapacity() const {
    return capacity;
}
void Wand::setCapacity(int size) {
    capacity = size;
}