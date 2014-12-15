#pragma once

enum class WeaponClass
{
    Fist,
    Pistol,
    MachineGun,
    Shotgun
};

class Weapon : public BaseObject<Weapon>
{
public:
    static std::shared_ptr<Weapon> Create(_In_ WeaponClass weaponClass, _In_ uint32_t damage, _In_ uint32_t ammo, _In_ uint32_t range, _In_ uint32_t ammoUsedPerShot);

    uint32_t GetMaxRange();
    uint32_t GetDamage();
    uint32_t GetAmmoCount();
    WeaponClass GetClass();
    uint32_t Use(_In_ uint32_t distance);

    void AddAmmo(_In_ uint32_t numAmmo);

private:
    Weapon(_In_ WeaponClass weaponClass, _In_ uint32_t damage, _In_ uint32_t ammo, _In_ uint32_t range, _In_ uint32_t ammoUsedPerShot);

private:
    WeaponClass _class;
    uint32_t _damage;
    uint32_t _ammo;
    uint32_t _range;
    uint32_t _ammoUsedPerShot;
};
