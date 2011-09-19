#include <RobotModelParser/urdf_model.h>
#include <stdio.h>

int main()
{
    urdf::URDFModel robot;

    printf("========== \n");
    printf("== TEST == \n");
    printf("========== \n");

    if(robot.initFile("../../urdf.xml"))
        printf("Successfully parsed the URDF xml file \n");
    else
        printf("Failed to parse the URDF xml file\n");

    if(robot.initFile("../../kuka.dae"))
        printf("Successfully parsed the collada file \n");
    else
        printf("Failed to parse the collada file\n");

    return 0;
}
