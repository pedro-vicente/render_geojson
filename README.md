# render_geojson
C++ geoJSON and topoJSON parser and rendering using the WxWidgets GUI library. 

# geoJSON specification
GeoJSON is a format for encoding a variety of geographic data structures.

http://geojson.org/

JSON parsing is done with gason

https://github.com/vivkin/gason

# lib_geojson 

GeoJSON parsing using lib_geojson

https://github.com/pedro-vicente/lib_geojson

# lib_topojson 

TopoJSON parsing using lib_topojson

https://github.com/pedro-vicente/lib_topojson

# UNIX build

Dependencies
------------

[wxWidgets](https://www.wxwidgets.org/)
wxWidgets is a library for creating graphical user interfaces for cross-platform applications.
<br /> 

Install dependency packages (Ubuntu):
<pre>
sudo apt-get install build-essential
sudo apt-get install autoconf
sudo apt-get install libwxgtk3.0-dev
</pre>

Get source:
<pre>
git clone https://github.com/pedro-vicente/render_geojson.git
</pre>

Build with:
<pre>
autoreconf -vfi
./configure
make
</pre>
