## CURL and OPENSSL installation
CURL should be compiled on machine and with openssl libs:
1. Install openssl by typing:
    ```bash
        # sudo apt-get install libssl-dev
    ```  
2. Identify the openssl location in machine:
    ```bash
        # openssl version -d
    ```  
    * Example:  
        "/usr/local/ssl"
3. Download and extract desired curl version(latest recommended) project, one posibility for example:
    ```bash
        # wget http://curl.haxx.se/download/curl-7.50.3.tar.gz
        # tar -xvf curl-7.50.3.tar.gz
    ```
4. Go to Curl project and compile with openssl option:
    ```bash
        # cd curl-7.50.3
        # ./configure --with-ssl='/usr/local/ssl'
        # make 
        # make install
    ```
    * Note: Default path for ```--with-ssl``` option is __/usr/local/ssl__ and this is the default path for openssl installation as well, but it can change, type ``` ./configure --help``` inside curl project for help in options of compilation.
