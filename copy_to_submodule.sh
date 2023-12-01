#!/bin/bash

# Percorsi alle directory del submodule
SUBMODULE_MODELS_PATH="coralmicro/models"
SUBMODULE_EXAMPLES_PATH="coralmicro/examples"
SUBMODULE_APPS_PATH="coralmicro/apps"
DESTINATION_PATH_1="coralmicro/examples/CMakeLists.txt"
DESTINATION_PATH_2="coralmicro/apps/CMakeLists.txt"

# Verifica che le directory del submodule esistano e siano state inizializzate
if [ ! -d "$SUBMODULE_MODELS_PATH" ] || [ ! -d "$SUBMODULE_EXAMPLES_PATH" ] || [ ! -d "$SUBMODULE_APPS_PATH" ]; then
    echo "Le directory del submodule non esistono. Assicurati di aver eseguito 'git submodule update --init'."
    exit 1
fi

# Percorso del file e della cartella nella tua repository principale che vuoi copiare
FILE_TO_COPY="models/model_edgetpu.tflite"
FOLDER_TO_COPY_1="examples/detect_gesture"
FOLDER_TO_COPY_2="apps/multicore_detect_gesture"
FILE_TO_OVERWRITE_1="examples/CMakeLists.txt"
FILE_TO_OVERWRITE_2="apps/CMakeLists.txt"


# Verifica che il file esista nella tua repository principale
if [ -f "$FILE_TO_OVERWRITE_2" ]; then
    # Copia e sovrascrive il file esistente nel submodule
    cp "$FILE_TO_OVERWRITE_2" "$DESTINATION_PATH_2"
    echo "Copiato e sovrascritto $FILE_TO_OVERWRITE_2 nel submodule."
else
    echo "Il file $FILE_TO_OVERWRITE_2 non esiste. Salto la copia."
fi

# Verifica che il file esista nella tua repository principale
if [ -f "$FILE_TO_OVERWRITE_1" ]; then
    # Copia e sovrascrive il file esistente nel submodule
    cp "$FILE_TO_OVERWRITE_1" "$DESTINATION_PATH_1"
    echo "Copiato e sovrascritto $FILE_TO_OVERWRITE_1 nel submodule."
else
    echo "Il file $FILE_TO_OVERWRITE_1 non esiste. Salto la copia."
fi

# Copia il file nel percorso dei modelli del submodule
if [ -f "$FILE_TO_COPY" ]; then
    cp "$FILE_TO_COPY" "$SUBMODULE_MODELS_PATH/"
    echo "Copiato $FILE_TO_COPY nel submodule."
else
    echo "Il file $FILE_TO_COPY non esiste. Salto la copia."
fi

# Copia la cartella negli esempi del submodule
if [ -d "$FOLDER_TO_COPY_1" ]; then
    # Usando -r per copiare ricorsivamente tutti i contenuti della cartella
    cp -r "$FOLDER_TO_COPY_1" "$SUBMODULE_EXAMPLES_PATH/"
    echo "Copiata la cartella $FOLDER_TO_COPY_1 nel submodule."
else
    echo "La cartella $FOLDER_TO_COPY_1 non esiste. Salto la copia."
fi

# Copia la cartella negli esempi del submodule
if [ -d "$FOLDER_TO_COPY_2" ]; then
    # Usando -r per copiare ricorsivamente tutti i contenuti della cartella
    cp -r "$FOLDER_TO_COPY_2" "$SUBMODULE_APPS_PATH/"
    echo "Copiata la cartella $FOLDER_TO_COPY_2 nel submodule."
else
    echo "La cartella $FOLDER_TO_COPY_2 non esiste. Salto la copia."
fi

echo "Operazione di copia completata."
