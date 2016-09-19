# LoRaWAN connected mailbox

Check out my [blog post](http://blog.inovia-conseil.fr/?p=262).

## Build and upload

```
pip install -U platformio
git clone https://github.com/ksahnine/mailbox-lora.git
cd mailbox-lora/objeniouskit/lora-notifier
pio lib install 374 1006
pio -f init -d . -b fio
```

