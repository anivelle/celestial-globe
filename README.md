# Celestial Globe Project

The goal of this project is to create a rotating celestial globe, akin to apps
such as Stellarium, but in a physical form factor.

Right now, because existing celestial globes don't really fit my requirements, 
I'm mapping the constellations based on the stars available in the Hipparcos
catalogue. I haven't included the data file in this repository since it's 50 
MB, but it can be downloaded [here](https://cdsarc.u-strasbg.fr/ftp/I/239/hip_main.dat).
No modification should be necessary, and it should be stored in the root of the
repository.

The file `constellations.csv` contains a list of stars in all 
[88 IAU constellations](https://en.wikipedia.org/wiki/IAU_designated_constellations) 
by their index in the Hipparcos catalogue. A couple stars are missing, but their
apparent magnitudes are small enough that they can be ignored while preserving
the recognizable image of the night sky. These indices were collected by
scraping the Wikipedia [lists of stars by constellations](https://en.wikipedia.org/wiki/Lists_of_stars_by_constellation).
The related code can be found in `constellation_parse.py`. I may use this code
to grab the names of the stars as well but it could be more complicated due to
Unicode characters.

