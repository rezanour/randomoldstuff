#pragma once

class Level
{
    struct LevelInfo
    {
        char Name[16];
        std::vector<int16_t> Data;
    };

public:
    Level(_In_z_ const wchar_t* contentRoot);

    uint32_t GetNumLevels() const;
    const int16_t* GetLevel(uint32_t levelIndex) const;

private:
    std::vector<LevelInfo> _levels;
};

