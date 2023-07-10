//Created by Srtyewiop

#include "PixelToMeshActor.h"
#include "ProceduralMeshComponent.h"
#include "SAdvancedRotationInputBox.h"

APixelToMeshActor::APixelToMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(FName("MeshComp"));
	Mesh->bUseComplexAsSimpleCollision=false;
}

void APixelToMeshActor::CreateMesh(){
    if(!GenerationTexture)
    {
	    return;
    }
    XSize = GenerationTexture->GetSizeX();
	YSize = GenerationTexture->GetSizeY();
	Mesh->ClearMeshSection(0);
	Mesh->ClearMeshSection(1);
	GetPixelDataAndFillMeshInfo();
	
	for (auto &Element : Vertices)
	{
    		Element = MeshRotation.RotateVector(Element);
			Element = Element + MeshOffset;
	}

	for (auto &Element : Normals)
	{
		Element = MeshRotation.RotateVector(Element);
	}
	
	Mesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), false);
	Mesh->UpdateMeshSection(0, Vertices, Normals, UV, TArray<FColor>(), TArray<FProcMeshTangent>());

	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
	DynMaterial->SetTextureParameterValue("Texture", GenerationTexture);
	Mesh->SetMaterial(0, DynMaterial);
	if(CollisionType!=None)
	{
		Mesh->ClearCollisionConvexMeshes();
		Mesh->AddCollisionConvexMesh(Collision);
	}else
	{
		Mesh->ClearCollisionConvexMeshes();
	}
	


	ClearInfo();
}

void APixelToMeshActor::GetPixelDataAndFillMeshInfo()
{
	const FColor* ImageData = static_cast<const FColor*>( GenerationTexture->GetPlatformData()->Mips[0].BulkData.LockReadOnly());

	if(CreateDetailedTopAndBottom==Planes)
	{
		if(bGenerateTop)
		{
			const FVector Top0 = FVector(0.0f, 0.0f, 0.0f);
			const FVector Top1 = FVector(1.0f*XSize, 0.0f, 0.0f)*MeshScale;
			const FVector Top2 = FVector(1.0f*XSize, 1.0f*YSize, 0.0f)*MeshScale;
			const FVector Top3 = FVector(0.0f, 1.0f*YSize, 0.0f)*MeshScale;

			Vertices.Add(Top0);
			Vertices.Add(Top1);
			Vertices.Add(Top2);
			Vertices.Add(Top3);

			UV.Add(FVector2D(0.0f, 0.0f));
			UV.Add(FVector2D(1.0f, 0.0f));
			UV.Add(FVector2D(1.0f, 1.0f));
			UV.Add(FVector2D(0.0f, 1.0f));

			Triangles.Add(2);
			Triangles.Add(1);
			Triangles.Add(0);

			Triangles.Add(3);
			Triangles.Add(2);
			Triangles.Add(0);
	
			Normals.Add(FVector(0,0,1));
			Normals.Add(FVector(0,0,1));
			Normals.Add(FVector(0,0,1));
			Normals.Add(FVector(0,0,1));

			if(CollisionType==FromTop)
			{
				Collision.Add(Top0);
				Collision.Add(Top1);
				Collision.Add(Top2);
				Collision.Add(Top3);
			}

		}
		if(bGenerateBottom)
		{
			const FVector Bottom0 = FVector(0.0f, 0.0f, -1)*MeshScale;
			const FVector Bottom1 = FVector(1.0f*XSize, 0.0f, -1)*MeshScale;
			const FVector Bottom2 = FVector(1.0f*XSize, 1.0f*YSize, -1)*MeshScale;
			const FVector Bottom3 = FVector(0.0f, 1.0f*YSize, -1)*MeshScale;

			Vertices.Add(Bottom0);
			Vertices.Add(Bottom1);
			Vertices.Add(Bottom2);
			Vertices.Add(Bottom3);

			UV.Add(FVector2D(0.0f, 0.0f));
			UV.Add(FVector2D(1.0f, 0.0f));
			UV.Add(FVector2D(1.0f, 1.0f));
			UV.Add(FVector2D(0.0f, 1.0f));

			Triangles.Add(4);
			Triangles.Add(5);
			Triangles.Add(6);

			Triangles.Add(4);
			Triangles.Add(6);
			Triangles.Add(7);

			Normals.Add(FVector(0,0,1)*-1.f);
			Normals.Add(FVector(0,0,1)*-1.f);
			Normals.Add(FVector(0,0,1)*-1.f);
			Normals.Add(FVector(0,0,1)*-1.f);

			if(CollisionType==FromTop)
			{
				Collision.Add(Bottom0);
				Collision.Add(Bottom1);
				Collision.Add(Bottom2);
				Collision.Add(Bottom3);
			}
		}
		

		
	}
	
	for(int32 X = 0; X < XSize; X++)
	{
		for (int32 Y = 0; Y < YSize; Y++)
		{
			FNeighbors N = FNeighbors();
			const bool pixel = ImageData[Y * GenerationTexture->GetSizeX() + X].A == 255;
			
			if (Y+1 <= YSize-1)
			{
				if(ImageData[(Y+1) * GenerationTexture->GetSizeX() + X].A == 255)
				N.DOWN=true;
			}

			if (Y-1 >= 0)
			{
				if(ImageData[(Y-1) * GenerationTexture->GetSizeX() + X].A == 255)
				N.UP=true;
			}

			if (X-1 >= 0)
			{
				if(ImageData[Y * GenerationTexture->GetSizeX() + (X-1)].A == 255)
				N.RIGHT=true;
			}

			if (X+1 <= XSize-1)
			{
				if(ImageData[Y * GenerationTexture->GetSizeX() + (X+1)].A == 255)
				N.LEFT=true;
			}
			
			if(pixel)
			{
				PixelData.Add(FVector2D(X, Y), N);
			} 
			
			
		}
	}
	
	GenerationTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

	
	if(CreateDetailedTopAndBottom==GreedyMesh)
	{
		TMap<FVector2D, FNeighbors> DataCopyForGreedyMesh = PixelData;
		GreedyMeshTopGeneration(DataCopyForGreedyMesh);
	}
	
	GreedyMeshSideGeneration();
}

void APixelToMeshActor::ClearInfo()
{
	Vertices.SetNum(0);
	Triangles.SetNum(0);
	UV.SetNum(0);
	Normals.SetNum(0);
	Collision.SetNum(0);
	PixelData.Empty();
}

void APixelToMeshActor::GreedyMeshSideGeneration()
{
	if(bGenerateSides)
	{
		for(int32 X = 0; X < XSize; X++)
		{
			for (int32 Y = 0; Y < YSize; Y++)
			{
				if(PixelData.Find(FVector2D(X, Y)))
				{
					FVector2D Current = FVector2D(X, Y);
					FVector2D Previous = FVector2D(X, Y-1);
					FVector2D Next = FVector2D(X, Y+1);
				
					if(!PixelData.Find(Current)->RIGHT)
					{
						int32 I0;
						int32 I1;
						if((PixelData.Find(Previous) && PixelData.Find(Previous)->RIGHT) || !PixelData.Find(Previous))
						{
							I0 = Vertices.Add(FVector(Current.X, Current.Y, 0.f)*MeshScale);
							I1 = Vertices.Add(FVector(Current.X, Current.Y, -1.f)*MeshScale);
							UV.Add(Current/FVector2D(XSize, YSize));
							UV.Add((Current+FVector2D(1.f, 0.f))/FVector2D(XSize, YSize));
							Normals.Add(FVector(-1,0,0));
							Normals.Add(FVector(-1,0,0));
						}
						if((PixelData.Find(Next) && PixelData.Find(Next)->RIGHT) || !PixelData.Find(Next))
						{
							int32 I3;
							int32 I2;
							I2 = Vertices.Add(FVector(Current.X, Current.Y+1, 0.f)*MeshScale);
							I3 = Vertices.Add(FVector(Current.X, Current.Y+1, -1.f)*MeshScale);
							UV.Add((Current+FVector2D(0.f, 1.f))/FVector2D(XSize, YSize));
							UV.Add((Current+FVector2D(1.f, 1.f))/FVector2D(XSize, YSize));

						
							Normals.Add(FVector(-1,0,0));
							Normals.Add(FVector(-1,0,0));

							Triangles.Add(I0);
							Triangles.Add(I1);
							Triangles.Add(I3);
						
							Triangles.Add(I0);
							Triangles.Add(I3);
							Triangles.Add(I2);
						}
					}

					if(!PixelData.Find(Current)->LEFT)
					{
						int32 I0;
						int32 I1;
						if((PixelData.Find(Previous) && PixelData.Find(Previous)->LEFT) || !PixelData.Find(Previous))
						{
							I0 = Vertices.Add(FVector(Current.X+1, Current.Y, 0.f)*MeshScale);
							I1 = Vertices.Add(FVector(Current.X+1, Current.Y, -1.f)*MeshScale);
							UV.Add((Current+FVector2D(1.f, 0.f))/FVector2D(XSize, YSize));
							UV.Add((Current)/FVector2D(XSize, YSize));

							Normals.Add(FVector(1,0,0));
							Normals.Add(FVector(1,0,0));
						
						}
						if((PixelData.Find(Next) && PixelData.Find(Next)->LEFT) || !PixelData.Find(Next))
						{
							int32 I3;
							int32 I2;
							I2 = Vertices.Add(FVector(Current.X+1, Current.Y+1, 0.f)*MeshScale);
							I3 = Vertices.Add(FVector(Current.X+1, Current.Y+1, -1.f)*MeshScale);
							UV.Add((Current+FVector2D(1.f, 1.f))/FVector2D(XSize, YSize));
							UV.Add((Current+FVector2D(0.f, 1.f))/FVector2D(XSize, YSize));

						
							Normals.Add(FVector(1,0,0));
							Normals.Add(FVector(1,0,0));

							Triangles.Add(I3);
							Triangles.Add(I1);
							Triangles.Add(I0);
						
							Triangles.Add(I2);
							Triangles.Add(I3);
							Triangles.Add(I0);
						}
					}
				}
			}
		}

		for(int32 Y = 0; Y < XSize; Y++)
		{
			for (int32 X = 0; X < YSize; X++)
			{
				if(PixelData.Find(FVector2D(X, Y)))
				{
					FVector2D Current = FVector2D(X, Y);
					FVector2D Previous = FVector2D(X-1, Y);
					FVector2D Next = FVector2D(X+1, Y);
				
					if(!PixelData.Find(Current)->UP)
					{
						int32 I0;
						int32 I1;
						if((PixelData.Find(Previous) && PixelData.Find(Previous)->UP) || !PixelData.Find(Previous))
						{
							I0 = Vertices.Add(FVector(Current.X, Current.Y, 0.f)*MeshScale);
							I1 = Vertices.Add(FVector(Current.X, Current.Y, -1.f)*MeshScale);
							UV.Add(Current/FVector2D(XSize, YSize));
							UV.Add((Current+FVector2D(0.f, 1.f))/FVector2D(XSize, YSize));
							Normals.Add(FVector(0,-1,0));
							Normals.Add(FVector(0,-1,0));
						}
						if((PixelData.Find(Next) && PixelData.Find(Next)->UP) || !PixelData.Find(Next))
						{
							int32 I3;
							int32 I2;
							I2 = Vertices.Add(FVector(Current.X+1, Current.Y, 0.f)*MeshScale);
							I3 = Vertices.Add(FVector(Current.X+1, Current.Y, -1.f)*MeshScale);
							UV.Add((Current+FVector2D(1.f, 0.f))/FVector2D(XSize, YSize));
							UV.Add((Current+FVector2D(1.f, 1.f))/FVector2D(XSize, YSize));
						
							Normals.Add(FVector(0,-1,0));
							Normals.Add(FVector(0,-1,0));

							Triangles.Add(I3);
							Triangles.Add(I1);
							Triangles.Add(I0);
						
							Triangles.Add(I2);
							Triangles.Add(I3);
							Triangles.Add(I0);
						}
					}

					if(!PixelData.Find(Current)->DOWN)
					{
						int32 I0;
						int32 I1;
						if((PixelData.Find(Previous) && PixelData.Find(Previous)->DOWN) || !PixelData.Find(Previous))
						{
							I0 = Vertices.Add(FVector(Current.X, Current.Y+1, 0.f)*MeshScale);
							I1 = Vertices.Add(FVector(Current.X, Current.Y+1, -1.f)*MeshScale);
							UV.Add((Current+FVector2D(0.f, 1.f))/FVector2D(XSize, YSize));
							UV.Add((Current)/FVector2D(XSize, YSize));
							Normals.Add(FVector(0,1,0));
							Normals.Add(FVector(0,1,0));
						}
						if((PixelData.Find(Next) && PixelData.Find(Next)->DOWN) || !PixelData.Find(Next))
						{
							int32 I3;
							int32 I2;
							I2 = Vertices.Add(FVector(Current.X+1, Current.Y+1, 0.f)*MeshScale);
							I3 = Vertices.Add(FVector(Current.X+1, Current.Y+1, -1.f)*MeshScale);
							UV.Add((Current+FVector2D(1.f, 1.f))/FVector2D(XSize, YSize));
							UV.Add((Current+FVector2D(1.f, 0.f))/FVector2D(XSize, YSize));

							Normals.Add(FVector(0,1,0));
							Normals.Add(FVector(0,1,0));

							Triangles.Add(I0);
							Triangles.Add(I1);
							Triangles.Add(I3);
						
							Triangles.Add(I0);
							Triangles.Add(I3);
							Triangles.Add(I2);
						}
					}
				}
			}
		}

		if(CollisionType==FromSides)
		{
			Collision = Vertices;
		}
	}
}

void APixelToMeshActor::GreedyMeshTopGeneration(TMap<FVector2D, FNeighbors> &DataCopy)
{
	if (DataCopy.Num() > 0)
	{
		FVector2D RectangleCorner = FVector2D(99999,99999);
		int32 RectY = 2147483647;
		int32 RectX = 2147483647;
		
		for(int32 X = 0; X < XSize; X++)
		{
			for(int32 Y = 0; Y < YSize; Y++)
			{
				if(X < RectangleCorner.X && DataCopy.Find(FVector2D(X, Y)))
				{
					RectangleCorner = FVector2D(X, Y);
				}
			}
		}

		for(int32 Y = RectangleCorner.Y; Y < YSize; Y++)
		{
			if(!DataCopy.Find(FVector2D(RectangleCorner.X, Y+1)))
			{
				if(RectY > Y)
				{
					RectY = Y;
				}
			}
		}

		for(int32 Y = RectangleCorner.Y; Y <= RectY; Y++)
		{
			for(int32 X = RectangleCorner.X; X < XSize; X++)
			{
				if(!DataCopy.Find(FVector2D(X+1, Y)))
				{
					if(RectX > X)
					{
						RectX = X;
					}
				}
			}
		}
		
		const FVector2D UVDecrease = FVector2D(XSize, YSize);
		
		if(bGenerateTop)
		{
			const int32 I0 = Vertices.Add(FVector(RectangleCorner.X, RectangleCorner.Y, 0.f)*MeshScale);
			const int32 I1 = Vertices.Add(FVector(RectangleCorner.X, RectY+1, 0.f)*MeshScale);
		
			const int32 I2 = Vertices.Add(FVector(RectX+1, RectY+1, 0.f)*MeshScale);
			const int32 I3 = Vertices.Add(FVector(RectX+1, RectangleCorner.Y, 0.f)*MeshScale);

			

			UV.Add(FVector2D(RectangleCorner.X, RectangleCorner.Y)/UVDecrease);
			UV.Add(FVector2D(RectangleCorner.X, RectY+1)/UVDecrease);
			UV.Add(FVector2D(RectX+1, RectY+1)/UVDecrease);
			UV.Add(FVector2D(RectX+1, RectangleCorner.Y)/UVDecrease);
		

			Triangles.Add(I0);
			Triangles.Add(I1);
			Triangles.Add(I2);
		
			Triangles.Add(I0);
			Triangles.Add(I2);
			Triangles.Add(I3);

			Normals.Add(FVector(0, 0, 1));
			Normals.Add(FVector(0, 0, 1));
			Normals.Add(FVector(0, 0, 1));
			Normals.Add(FVector(0, 0, 1));

			if(CollisionType==FromTop)
			{
				Collision.Add(Vertices[I0]);
				Collision.Add(Vertices[I1]);
				Collision.Add(Vertices[I2]);
				Collision.Add(Vertices[I3]);
			}
		}
		
		
		if(bGenerateBottom)
		{
			const int32 E0 = Vertices.Add(FVector(RectangleCorner.X, RectangleCorner.Y, -1.f)*MeshScale);
			const int32 E1 = Vertices.Add(FVector(RectangleCorner.X, RectY+1, -1.f)*MeshScale);
		
			const int32 E2 = Vertices.Add(FVector(RectX+1, RectY+1, -1.f)*MeshScale);
			const int32 E3 = Vertices.Add(FVector(RectX+1, RectangleCorner.Y, -1.f)*MeshScale);
		

			UV.Add(FVector2D(RectangleCorner.X, RectangleCorner.Y)/UVDecrease);
			UV.Add(FVector2D(RectangleCorner.X, RectY+1)/UVDecrease);
			UV.Add(FVector2D(RectX+1, RectY+1)/UVDecrease);
			UV.Add(FVector2D(RectX+1, RectangleCorner.Y)/UVDecrease);

			Triangles.Add(E2);
			Triangles.Add(E1);
			Triangles.Add(E0);
		
			Triangles.Add(E3);
			Triangles.Add(E2);
			Triangles.Add(E0);

			Normals.Add(FVector(0, 0, -1));
			Normals.Add(FVector(0, 0, -1));
			Normals.Add(FVector(0, 0, -1));
			Normals.Add(FVector(0, 0, -1));

			if(CollisionType==FromTop)
			{
				Collision.Add(Vertices[E0]);
				Collision.Add(Vertices[E1]);
				Collision.Add(Vertices[E2]);
				Collision.Add(Vertices[E3]);
			}
		}
		

		for(int32 Y = RectangleCorner.Y; Y <= RectY; Y++)
		{
			for(int32 X = RectangleCorner.X; X <= RectX; X++)
			{
				DataCopy.Remove(FVector2D(X,Y));
			}
		}

		

		GreedyMeshTopGeneration(DataCopy);
	}
	else
	{
		return;
	}
}
