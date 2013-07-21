#!/bin/sh

QTCREATOR_CONFIG_PATH="$HOME/.config/QtProject/qtcreator/templates/wizards"


if [ ! -e $QTCREATOR_CONFIG_PATH ]; then
  echo "Creating the QtCreator config path: $QTCREATOR_CONFIG_PATH"
  mkdir -p $QTCREATOR_CONFIG_PATH
fi

for i in *; do
    if [ -d $i ]; then
        echo "Creating $QTCREATOR_CONFIG_PATH/$i"
        ln -sf $PWD/$i $QTCREATOR_CONFIG_PATH
    fi
done
