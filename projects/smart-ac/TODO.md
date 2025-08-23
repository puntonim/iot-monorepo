Saturday 2025.08.23 14:32:12

Questo codice ha funzionato bene per diversi giorni e in diverse occasione durante
l'estate del 2025 per controllare il climatizzatore.
È un codice che ho scritto di fretta, per poterlo utilizzare velocemente, perchè
faceva molto caldo.

Ora bisogna trasformare questo codice in un progetto fatto bene, con le seguenti cose:

- usare il template 3-big-project-w-display-and-rotary-template
  (o un template anche con il menu)
- devo vedere la target T sul display e poterla cambiare con un rotary encoder
- devo vedere la T e umidità della stanza sul display
- magari un countdown timer che mi permette di spegnere automaticamente di notte

Nota: in altri progetti (come nel template 3-big-project-w-display-and-rotary-template)
ho un problema con le HTTP requests in RemoteLog: ogni tanto falliscono. Mentre questo
codice ha funzionato per molti giorni senza che le request abbiano fallito. Non
capisco perchè ma investighero in: https://github.com/puntonim/arduino-monorepo/tree/main/SW__WIFI/wifi-http-request-w-lib
