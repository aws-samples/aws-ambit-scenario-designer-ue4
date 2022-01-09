# Welcome to AWS Ambit Scenario Designer for Unreal Engine 4

AWS Ambit Scenario Designer for Unreal Engine 4 (Ambit) is a suite of tools to streamline 3D content creation at scale for autonomous vehicle and robotics simulation applications. From procedural placement of obstacles to automatic generation of whole cities, Ambit can help you quickly create the 3D environments and scenarios you need for your simulations or other 3D applications.

![Ambit sample image](images/AmbitBanner.jpg)

Ambit is implemented as a plugin for the Windows version of Unreal Engine 4 (UE4). However, the content you create with Ambit can be exported and used with other versions of UE4 (Mac, Linux) or even non-UE4 simulators and 3D tools.

To learn what Ambit has to offer, check out the [Feature Overview](./concepts-and-features).

Ambit has been tested for compatibility with [Unreal Engine 4.27](https://docs.unrealengine.com/en-US/Support/Builds/ReleaseNotes/4_27/index.html).



> 📣 **Attention:** During the usage of this plugin tool Ambit will collect generic metrics to inform AWS about usage patterns and performance of Ambit's individual features. These metrics contain no personally identifiable information and are not shared or transferred.
> 
> If you prefer to opt out from this metrics collection you can do so by making a small code modification and compiling the Ambit plugin from source. Within the Ambit plugin folder, edit the C++ header file `Source/Ambit/Utils/UserMetricsSubsystem.h` to make the `RecordUserMetrics` field false. Full details on how to compile Ambit from source can be found in ["Appendix: Contributing to Ambit Development"](appendix-contributing/).

