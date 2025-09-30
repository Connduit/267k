class AntiDebug
{
public:
    AntiDebug();
    ~AntiDebug();

    bool debugFlags();
    bool objectHandles();
    bool exceptions();
    bool timing();
    bool processMemory();
    bool assemblyInstructions();
    bool directDebug();
    bool misc();

private:
};
