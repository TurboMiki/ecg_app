### Instrukcja obsługi dockera na Windowsie

1. Pobierz Docker Desktop - najlepiej z tego linku: https://docs.docker.com/desktop/setup/install/windows-install/ Instalacja zrobi się sama, uiruchom tylko plik docker-install.exe.
2. Pobierz Windows Xserver, żeby UI dobrze się wyśiwetlał. Wchodzisz w terminal windowsa (cmd), wpisujesz winget install marha.VcXsrv i się pobiera.
3. W folderze, w którym masz repo z aplikacją, upewnij się że masz ostatnią wersję kodu, w dowolnym terminalu (np. git bash), i stwórz nowego brancha z maina, wyczyść dockera dla pewności:
    - git pull
    - git checkout main
    - git branch *nazwa twojego nowego bracha*
    - mkdir build
    - docker builder prune
    - docker system prune -a --volumes
4. Uruchom serwer XServer:
    - kliknij ikonkę XServer, może włączyć się samo po instalacji
    - wybierz "Multiple windows"
    - ustaw "Display number" na 0
    - Wybierz "Start no client"
    - zaznacz "Disable access control" i "Native opengl"
    - Kliknij "Save configuration"
    - kliknij "Finish", serwer będzie działać
5. Wejdź w dowolny terminal, Docker Desktop ma wbudowany, wejdź do folderu z repo z aplikacją, i wpisz komendy:
    - docker compose build --no-cache
    - docker compose up ecg-dev
6. Powinno otworzyć się okno z aplikacją i systemem plików kontenera
7. Przy kolejnym uruchomieniu powinno wystarczyć uruchomienie ponownie kontenera:
   - docker container ls --all
   - Znajdź i skopiuj z listy ID kontenera (powinien być jeden)
   - docker container start *id kontenera*
8. Pamiętaj, że po każdym git pull musisz zbudować kontener od nowa!:
   - docker compose build --no-cache
   - docker compose up ecg-dev
9. Jeśli cokolwiek nie działa, pisz do Mikołaja albo na grupce
