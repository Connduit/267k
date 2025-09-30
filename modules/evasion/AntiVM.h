class AntiVM
{
public:
    AntiVM();
    ~AntiVM();

    bool filesystem();
    bool registry();
    bool genericOsQueries();
    bool globalOsObjects();
    bool UIArtifacts();
    bool OsFeatures();
    bool processes();
    bool network();
    bool cpu();
    bool hardware();
    bool firmwareTables();
    bool hooks();
    bool timing();
    bool WMI();
    bool humanLikeBehavior();
private:
};
