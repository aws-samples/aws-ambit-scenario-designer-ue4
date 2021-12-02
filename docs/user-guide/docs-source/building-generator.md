![Ambit Building Generator output](./images/city-generation/BuildingGeneratorOutput.jpg)

## Using the Ambit Building Generator

Using the UE editor's Content Browser, navigate to the _**Ambit Content > ProceduralTools**_ folder. Then, right-click on the **AmbitBuildingGenerator** and select **"Instantiate at the origin"**.

> ✏️ **Note:** If you don't see the **Ambit Content** folder listed in the Content Browser you will need to enable the "Show Plugin Content" option using the Content Browser's View Options menu. ![View Options menu](images/city-generation/ViewOptionsMenu.png) You may also need to toggle the "Show/Hide Sources Panel" icon. ![Show Sources icon](images/city-generation/ShowSourcesIcon.png)

There will be a small delay while the Houdini Engine starts in the background and then initializes the asset.

Ensure that the new **AmbitBuildingGenerator** instance is selected in the World Outliner panel. Then, find the section of the Details panel titled _**Houdini Parameters**_. Import an OSM file that you downloaded previously by clicking the `…` icon next to the _**OSM File**_ parameter field.

![OSM File parameter](images/city-generation/BuildingGen_OSMParam.png)

Building generation will start automatically. Generation could take several minutes depending on the size and complexity of the OSM building data.

