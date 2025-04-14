--- Workflow Logic ---

- Determines data paths

- Downloads JSON files via cURL

- Processes and compares packages

- Saves results to comparison_result.json

- Automatically download up-to-date data

--- File Organization ---

data/

-----p10/

---------p10_packages.json

-----Sisyphus/

---------sisyphus_packages.json

-----result/

---------comparison_result.json

---Structure of comparison_result.json---

{

    "--stats--": {
    
        "p10_only_count": 0,
        
        "sisyphus_only_count": 0,
        
        "up_ver_relise_in_sisyphus_count": 0,

        "total_p10_packages": 0,
        
        "total_sisyphus_packages": 0
        
    }
    
    "p10_only": [
    
        {
        
        "name": "string",
        
        "arch": "string",
        
        "version": "string",
        
        "release": "string"
        
        }
        
    ],
    
    "sisyphus_only": [
    
        {
        
            "name": "string",
            
            "arch": "string",
            
            "version": "string",
            
            "release": "string"
            
        }
        
    ],
    
    "up_ver_relise_in_sisyphus": [
    
        {
        
            "name": "string",
            
            "arch": "string",
            
            "p10_version": "string-string",
            
            "sisyphus_version": "string-string"
            
        }
        
    ]
    
}
