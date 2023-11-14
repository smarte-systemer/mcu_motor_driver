namespace TurretSyndrome
{
    unsigned long baud{115200};
    unsigned int ticks = 761;
    namespace Azimuth
    {
      const uint8_t direction{6};
      const uint8_t pulse{5};
    };
    namespace Pitch
    {
      const uint8_t direction{3};
      const uint8_t pulse{4};
    };
    namespace Trigger
    {
      const uint8_t input{9};
      const uint8_t output{8};
    };
};