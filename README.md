# MCU Motor Driver

Listens for JSON messages on Serial, uses timer to bitbang signals for [Nema TB6600](https://www.dfrobot.com/product-1547.html). 
## Motor position example
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
## Trigger mechanism
```json
{
    "T":1
}
```