/*===========================================================================*\
 | 
 |  FILE:   DoExport.cpp
 |       A simple exporter that scans the scene for our custom data
 |       and if found, exports it to an ASCII file
 |       3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |       Developer Consulting Group
 |       Copyright(c) Discreet 1999
 |
 |  HIST:   Started 16-4-99
 | 
\*===========================================================================*/
/*===========================================================================*\
 | This is where all the finding of exportable Custom Vertex Data gets done
 | All the main processing is done on each node during the enumeration
\*===========================================================================*/


#include "CVDExport.h"

#include "3dsmaxport.h"


/*===========================================================================*\
 |  Determine what options we support
\*===========================================================================*/

BOOL CVDExporter::SupportsOptions(int ext, DWORD options)
{
   switch(ext) {
      case 0:
         if(options & SCENE_EXPORT_SELECTED) return TRUE;
         else return FALSE;
         break;
      }
   return FALSE;
}



/*===========================================================================*\
 |  Preferences dialog handler
\*===========================================================================*/

static INT_PTR CALLBACK PrefsDlgProc(
      HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
   {
   CVDExporter *se = DLGetWindowLongPtr<CVDExporter*>(hWnd);
   if (!se && msg!=WM_INITDIALOG) return FALSE;

   switch (msg) {
      case WM_INITDIALOG:  
         // Update class pointer
         se = (CVDExporter*)lParam;
         DLSetWindowLongPtr(hWnd, lParam);

         // Setup preferences initial state
         SetCheckBox(hWnd,IDC_SEARCH1+se->searchtype,TRUE);
         break;

      case WM_DESTROY:
         break;

      case WM_COMMAND:
         switch (LOWORD(wParam)) {
            case IDC_CANCEL:
               EndDialog(hWnd,1);
               break;

            case IDC_OK:
               // Retrieve preferences
               se->searchtype = GetCheckBox(hWnd,IDC_SEARCH1)?0:1;

               EndDialog(hWnd,0);
            break;
         }
         break;

      default:
         return FALSE;
      }  
   return TRUE;
} 



/*===========================================================================*\
 |  For every node we get, try and find some custom data
 | 'channel' method will just see if the channel needed is present
 | 'modifier' method will see if the modifier exists in the modstack 
\*===========================================================================*/

// from the CVD exporter
#define MY_CHANNEL      5

BOOL CVDExporter::nodeEnum(INode* node,Interface *ip) 
{
   if(exportSelected && node->Selected() == FALSE)
      return TREE_CONTINUE;

   // Check for user cancel
   if (ip->GetCancel())
      return FALSE;


   if(!exportSelected || node->Selected()) 
   {
      // Check to see if we can see the modifier first
      if(searchtype==0)
      {
         fileStream.Printf(_T("Checking Node : %s\n"),node->GetName());

         BOOL foundMod = FALSE;

         // Get an object reference from the node
         Object *pObj = node->GetObjectRef();
         IDerivedObject *pDerObj = NULL;
         Modifier *pMod = NULL;

         // Is it a derived object?
         if( pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID) 
         {
            pDerObj = (IDerivedObject *) pObj;

            // Cycle through modifiers, looking for ours by Class_ID
            for(int i = 0 ; i < pDerObj->NumModifiers() ; i++ )
            {
                  pMod = pDerObj->GetModifier(i);
                  if(pMod->ClassID()==Class_ID(0x4fd31c27, 0xa9323c7)) foundMod = TRUE;
            }
         }

         // Did we find a modifier we can use?
         if(foundMod)
         {
            // Collapse object to usable TriObject mesh
            ObjectState os = node->EvalWorldState(ip->GetTime());
            if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID 
               && os.obj->IsRenderable() 
               && os.obj->IsDeformable()) 
            {
               TriObject *tobj = (TriObject *)os.obj;
               Mesh* mesh = &tobj->GetMesh();
               int numVert = mesh->getNumVerts();

               // Check for channel existance
               if(mesh->vDataSupport(MY_CHANNEL))
               {
                  // Get the float array of values
                  float *outDV = mesh->vertexFloat(MY_CHANNEL);
                  for(int i=0;i<numVert;i++)
                  {
                     fileStream.Printf(_T("Vertex %i : CVD [%.2f]\n"),i,outDV[i]);
                  }
               }
            }

         }
         fileStream.Printf(_T("\n"));
      }
      // Do a channel search instead - just collapse and check channel
      // You could change the value of MY_CHANNEL to be 0 and use a channel
      // search to dump out the soft selection of the nodes in the scene (for example)
      else
      {
         fileStream.Printf(_T("Checking Node : %s, Channel %i\n"),node->GetName(),MY_CHANNEL);
            
         ObjectState os = node->EvalWorldState(ip->GetTime());
         if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID 
            && os.obj->IsRenderable() 
            && os.obj->IsDeformable()) 
         {
            TriObject *tobj = (TriObject *)os.obj;
            Mesh* mesh = &tobj->GetMesh();
            int numVert = mesh->getNumVerts();

            if(mesh->vDataSupport(MY_CHANNEL))
            {
               float *outDV = mesh->vertexFloat(MY_CHANNEL);
               for(int i=0;i<numVert;i++)
               {
                  fileStream.Printf(_T("Vertex %i : CVD [%.2f]\n"),i,outDV[i]);
               }
            }
         }

         fileStream.Printf(_T("\n"));
      }
   }


   // Recurse through this node's children, if any
   for (int c = 0; c < node->NumberOfChildren(); c++) {
      if (!nodeEnum(node->GetChildNode(c), ip))
         return FALSE;
   }


   return TRUE;
}



/*===========================================================================*\
 |  Do the export to the file
\*===========================================================================*/

int   CVDExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
   // Set local interface pointer
   ip = i;

   // load the configuration from disk
   // so that it can be used in our dialog box
   if(!LoadExporterConfig()) return 0;

   if(!suppressPrompts)
   {

      // Show preferences setup dialog
      int res = DialogBoxParam(
         hInstance,
         MAKEINTRESOURCE(IDD_CVDEXP),
         i->GetMAXHWnd(),
         PrefsDlgProc,
         (LPARAM)this);

      // User clicked 'Cancel'
      if(res!=0) return 0;

   }

   exportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;

   	Interface14 *iface = GetCOREInterface14();
	UINT codepage  = iface-> DefaultTextSaveCodePage(true);
	if(!fileStream.Open(name, false, MaxSDK::Util::TextFile::Writer::WRITE_BOM | codepage )) return 0;

   // Print out a title for the file header
	fileStream.Printf(_T("Results of the Custom Data Per Vertex Search\n"));

   // Print out the state of the search parameter
   fileStream.Printf(_T("Search Type Chosen: %s\n\n"), searchtype? _T("Channel"):_T("Modifier"));


   // Simple root node -> children enumeration
   // This will get the root node, and then cycle through its children (ie, the basic scene nodes)
   // It will then recurse to search their children, and then their children, etc
   int numChildren = i->GetRootNode()->NumberOfChildren();

   for (int idx=0; idx<numChildren; idx++) {
      if (i->GetCancel())
         break;
      nodeEnum(i->GetRootNode()->GetChildNode(idx), i);
   }

   fileStream.Close();


   // Save the current configuration back out to disk
   // for use next time the exporter is run
   SaveExporterConfig();


   return 1;
}
