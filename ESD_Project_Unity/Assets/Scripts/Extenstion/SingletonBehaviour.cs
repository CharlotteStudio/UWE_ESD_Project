using UnityEngine;

public class SingletonBehaviour<T> : MonoBehaviour where T : SingletonBehaviour<T>
{
    private static T _instance;
    public static T Instance  
    {  
        get  
        {  
            if (_instance == null)  
            {
                _instance = FindObjectOfType<T>(); 
                if (_instance == null) 
                {
                    var obj = new GameObject(typeof(T).Name);
                    _instance = obj.AddComponent<T>();  
                }
            }
            return _instance;  
        }
    }
    
    protected virtual void Awake()
    {
        if (_instance != null && _instance != this)
        {
            Destroy(gameObject);
            return;
        }

        _instance = FindObjectOfType<T>();
        DontDestroyOnLoad(gameObject);
    }
}
