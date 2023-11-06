# MCU Motor Driver

Listens for JSON messages on Serial, uses timer to bitbang signals for [Nema TB6600](https://www.dfrobot.com/product-1547.html). 
## Message example
```json
{
    "A":
    {
        "Steps": 100,
        "Direction": 1
    },
    "B": 
    {
        "Steps": 200,
        "Direction": 0
    }
}
```