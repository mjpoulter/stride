# Strides++

![face](storeInfo/screenshots/basalt/aboveTarget.png)

This watchface is a "enhanced" version of pebble [Stride](https://apps.getpebble.com/en_US/application/56b15c5c9c4b20ed5300006c) watchface. Changes include:
* Icon to show phone connection status.
* Battery icon. Charge is shown as solid bar. When charging will show a different icon.
* Date
* When connected/disconnected from the phone will vibrate
* The Daily average where replaced with a goal.
* Local weather
* Daily goal is configurable

## Peble Time
![Above target](storeInfo/screenshots/basalt/animation.gif)![Below target](storeInfo/screenshots/basalt/belowTarget.png)![Above daily goal](storeInfo/screenshots/basalt/aboveDailyGoal.png)![Alternate icones](storeInfo/screenshots/basalt/alternateIcons.png)

## Pebble Round
![Above complete](storeInfo/screenshots/chalk/temperature.png)![Below complete](storeInfo/screenshots/chalk/belowTarget.png)

## Peble and Pebble 2
![Above complete](storeInfo/screenshots/aplite/temperature.png)![Below complete](storeInfo/screenshots/aplite/aboveComplete.png)

You need to add weather support before the App can be compile. Run the following command in the terminal

```$pebble package install pebble-generic-weather```

The code is based on [Mitchell Roy](mailto:mitchell@pabble.com) and [Chris Lewis](mailto:Bonsitm@gmail.com) [health-watchface](https://github.com/pebble-examples/health-watchface.git). Health-watchface is a reference copy of the official pebble Stride health watchface.
