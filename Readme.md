# Exif Data Reader

Recently I was given a coding challenge related to Exif meta in photos. The idea behind the challenge was that a lot of pictures taken are from smartphones that include gps location data in the photo. This presents privacy concerns as users are unaware that their location information is easily attainable from the photo's meta data. For instance even if a photo taken at someone's house is uploaded to facebook and the photo isn't geotagged on facebook, if the photo is downloaded and analyzed with a data reader then the location of the user's house could be obtained. I found this extremely interesting and was excited to do this challenge (despite there already being a wealth of tools out there that do this very thing). The challenge instructions are below.

> An image’s metadata is a goldmine of information. It can contain data about the model of the camera
that took the picture or the lighting details when the picture was taken. Over the past few years, privacy
concerns were raised with sites ranging from Facebook to 4chan after images that included GPS data
were uploaded and users were subsequently tracked in the real world. This challenge will involve
traversing such a structure to extract GPS data.

> You are given a block of EXIF data (challenge3.bin) from an image and asked to determine where the
image was taken. We expect our developers to be able to handle a wide variety of formats, including the
obsolete, the esoteric, and the customized, and we want to see that you can follow a specification. As
such, the use of EXIF specific libraries is prohibited for this challenge.

> To complete this challenge, your code should:
> * Parse the relevant TIFF structures in the file (see the included TIFF6.pdf specification).
>   * ImageFileHeader
>   * ImageFileDirectory
>   * ImageFileEntry
> * Extract the entries (IFEs) that pertain to GPS info (see table below).
> * Convert the GPS info into coordinates (degrees/minutes/seconds to decimal degrees).

> Once your program gives you the coordinates, you should manually look them up. Where was this image
taken? Use property_tags.txt to help you find other IFEs. What else can you tell us? Submit a file in
addition to your program with the location (a name, not coordinates) and any other information you
find.

> Your program should accept a file path via standard in and report the coordinates to standard out. You
are not required to write a component to do the GPS lookups automatically.

## How to run
Copy the repo and run:
~~~
make
./exifRead challenge03.bin
~~~

## To do (future things I am going to add)
- Need to create a search function that will return value of an inputted tag
- Definitely need to figure out how use c++ generics (or maybe templates?) properly to reduce a lot of redundant parse code
- Print all information parsed if no tag given
- Modify to extract data from real images and not just the metadata itself
