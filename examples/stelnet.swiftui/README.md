# stelnet/examples/stelnet.swiftui

A sample SwiftUI app using [Stelnet](https://github.com/CrispStrobe/Stelnet) to do voice-to-text transcriptions.
See also: [stelnet.objc](https://github.com/CrispStrobe/Stelnet/tree/main/examples/stelnet.objc).

### Building
 First stelnet need to be built and a XCFramework needs to be created. This can be done by running
 the following script from the stelnet project root:
 ```console
 $ ./build-xcframework.sh
 ```

Note: if you get the error "iphoneos is not an iOS SDK" then you probably need to run this command first:
```console
sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer
```

 Open `stelnet.swiftui.xcodeproj` project in Xcode and you should be able to build and run the app on
 a simulator or a real device.

 To use the framework with a different project, the XCFramework can be added to the project by
 adding `build-apple/stelnet.xcframework` by dragging and dropping it into the project navigator, or
 by manually selecting the framework in the "Frameworks, Libraries, and Embedded Content" section
 of the project settings.

### Usage

1. Select a Whisper model from [ggerganov/whisper.cpp on Hugging Face](https://huggingface.co/ggerganov/whisper.cpp/tree/main).[^1]
2. Add the model to `stelnet.swiftui.demo/Resources/models` **via Xcode**.
3. Select a sample audio file (for example, [jfk.wav](https://raw.githubusercontent.com/CrispStrobe/Stelnet/main/samples/jfk.wav)).
4. Add the sample audio file to `stelnet.swiftui.demo/Resources/samples` **via Xcode**.
5. Select the "Release" [^2] build configuration under "Run", then deploy and run to your device.

**Note:** Pay attention to the folder path: `stelnet.swiftui.demo/Resources/models` is the appropriate directory to place resources whilst `stelnet.swiftui.demo/Models` is related to actual code.

### Core ML support
1. Follow all the steps in the `Usage` section, including adding the ggml model file.  
The ggml model file is required as the Core ML model is only used for the encoder. The
decoder which is in the ggml model is still required.
2. Follow the [`Core ML support` section of readme](../../README.md#core-ml-support) to convert the
model.
3. Add the Core ML model (`models/ggml-base.en-encoder.mlmodelc/`) to `stelnet.swiftui.demo/Resources/models` **via Xcode**.

When the example starts running you should now see that it is using the Core ML model:
```console
whisper_init_state: loading Core ML model from '/Library/Developer/CoreSimulator/.../StelnetDemo.app/models/ggml-base.en-encoder.mlmodelc'
whisper_init_state: first run on a device may take a while ...
whisper_init_state: Core ML model loaded
```

[^1]: I recommend the tiny, base or small models for running on an iOS device.

[^2]: The `Release` build can boost performance of transcription. In this project, it also added `-O3 -DNDEBUG` to `Other C Flags`, but adding flags to app proj is not ideal in real world (applies to all C/C++ files), consider splitting xcodeproj in workspace in your own project.

![image](https://user-images.githubusercontent.com/1991296/212539216-0aef65e4-f882-480a-8358-0f816838fd52.png)
