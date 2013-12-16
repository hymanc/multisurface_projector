//from http://www.pcl-users.org/PolygonMesh-trying-to-render-using-OpenGL-td3551369.html

using namespace pcl;

struct Vertex { 
  Eigen::Vector3f pos; 
  Eigen::Vector3f normal; 
  Eigen::Vector3f color; 
}; 

{
	//...

	pcl::PointCloud<pcl::PointXYZRGB> vertices; 
	pcl::PolygonMesh mesh; 
	//pcl::io::loadPolygonFile("mesh.ply", mesh); 
	pcl::fromROSMsg(mesh.cloud, vertices); 

	std::vector <Vertex> verts; 
	std::vector <int> indices; 

	for (size_t i=0; i<mesh.polygons.size(); ++i) { 
		for (size_t j=0; j<mesh.polygons[i].vertices.size(); ++j) { 
			//std::cout << mesh.polygons[i].vertices[j] << std::endl;
			Vertex v;
			v.pos = mesh.polygons[i].vertices[j];
			verts.push_back(v);

			//How do I get indicies?

		} 
	} 

	//...

}
/*---------------------------------------------------------------------------------*/
//Using glut_ply.cpp
{
	//...

	char * filename = "~/things/stuff/dohickey.ply";

	ModelPly mply;

	if(mply.Load(filename)){
		return -1; //error
	}

	mply.Draw(); //it's done for us
	
	//...

}
