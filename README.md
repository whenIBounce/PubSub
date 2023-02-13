# PubSub

## How to run:

Open a terminal window:

1. ```make -f Makefile.communicate```

2. ```gcc -o communicate_client2 communicate_client2.c communicate_clnt.c communicate_xdr.c -pthread```

3. ```./communicate_server```

Then open another terminal window:
```./communicate_client localhost```

Then open another terminal window:
```./communicate_client2 localhost```
