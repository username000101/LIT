# Setting up and configuring LIT

### Attention: LIT is still "raw", so this article may change repeatedly



##### Working with the configuration

- **Restoring the standard configuration**: to restore the configuration, use the --reinit option when starting LIT, example: "sudo ./LIT --reinit"

##### Customization of LIT

- **Custom prefix for commands**: By default, LIT uses '.' as a prefix for commands, i.e.: ".module_name", however, you can override it in the configuration:

  Open the LIT configuration(it is located at the path LIT_INSTALL_PREFIX/config.json) (if you do not remember which directory you specified for installation, run LIT with the --info parameter, this one will show some information about LIT, example: "sudo ./LIT --info"

  Add the appearance object, and the "command_prefix" key to it, specify your command prefix as the value, example: "command_prefix": "$", In the end the configuration file should look something like this:

  ```json
  {
      "dir": "/path/to/lit/dir",
      "version": "version",
      "td_settings": {
          "api_id": "your api_id",
          "api_hash": "your api_hash"
      },
      "modules": "/path/to/lit/dir/modules.json",
      "appearance": {
          "command_prefix": "$"
      }
  }
  ```

-  **Blocking requests**: LIT allows you to block requests to the TelegramAPI, this eliminates the possibility for modules to send blocked requests

  To block a request, we need to know its id (you can find it in the tdlib source files), the example will show the blocking of the getChat request (its id is 1866601536). To mark a request as blocked, you need to add a blocked_requests array to the configuration file in the td_settings object. In the end, your configuration file should look something like this:

  ```json
  {
      "dir": "/path/to/lit/dir",
      "version": "version",
      "td_settings": {
          "api_id": "your api_id",
          "api_hash": "your api_hash",
          "blocked_requests": [
              1866601536
          ]
      },
      "modules": "/path/to/lit/dir/modules.json",
      "appearance": {
          "command_prefix": "$"
      }
  }
  ```

  now, when you try to call a getChat request, LIT returns an empty response

