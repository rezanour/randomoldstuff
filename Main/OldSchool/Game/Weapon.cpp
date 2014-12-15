#include "PreComp.h"
#include "Weapon.h"

_Use_decl_annotations_
std::shared_ptr<Weapon> Weapon::Create(WeaponClass weaponClass, uint32_t damage, uint32_t ammo, uint32_t range, uint32_t ammoUsedPerShot)
{
    return std::shared_ptr<Weapon>(new Weapon(weaponClass, damage, ammo, range, ammoUsedPerShot));
}

_Use_decl_annotations_
Weapon::Weapon(WeaponClass weaponClass, uint32_t damage, uint32_t ammo, uint32_t range, uint32_t ammoUsedPerShot) :
    _class(weaponClass), _damage(damage), _ammo(ammo), _range(range), _ammoUsedPerShot(ammoUsedPerShot)
{
}

uint32_t Weapon::GetDamage()
{
    return _damage;
}

uint32_t Weapon::GetAmmoCount()
{
    return _ammo;
}

uint32_t Weapon::GetMaxRange()
{
    return _range;
}

WeaponClass Weapon::GetClass()
{
    return _class;
}

_Use_decl_annotations_
void Weapon::AddAmmo(uint32_t numAmmo)
{
    _ammo += numAmmo;
}

_Use_decl_annotations_
uint32_t Weapon::Use(uint32_t distance)
{
    UNREFERENCED_PARAMETER(distance); // TODO: for some weapons, distance should be a scaler to the amount of damage

    uint32_t damageToTarget = 0;
    if (_ammo > 0)
    {
        damageToTarget = _damage;
        _ammo -= min(_ammo, _ammoUsedPerShot);
    }

    return damageToTarget;
}
