using System;
using TMPro;
using System.Threading.Tasks;
using UnityEngine;
using UnityUISample;
using FirebaseTools;

public class UIManager : MonoBehaviour
{
    private const string AlarmSettingPath = "/MSP430_Project/Alarm_Setting";
    private const string TimeZonePath = "/MSP430_Project/TimeZone";

    private const string PWKey = "p_ey";
    private const string AccountKey = "a_ey";
    
    [Header("Page")]
    [SerializeField] private RectTransform _loginPage;
    [SerializeField] private RectTransform _mainPage;
    [SerializeField] private PopupDialog _popupDialog;
    
    [Header("Login Page")]
    [SerializeField] private TMP_InputField _accountInputField;
    [SerializeField] private TMP_InputField _pwInputField;
    [SerializeField] private SimpleButton _simpleButton;
    
    [Header("Main Page")]
    [SerializeField] private TMP_Text _timerText;
    
    [SerializeField] private TMP_Text _alarmText;
    [SerializeField] private TMP_InputField _alarmInputField;
    [SerializeField] private SimpleButton _alarmButton;
    
    [SerializeField] private TMP_Text _timeZoneText;
    [SerializeField] private TMP_InputField _timeZoneInputField;
    [SerializeField] private SimpleButton _timeZoneButton;
    
    [SerializeField] private SimpleButton _renewButton;

    [SerializeField] private int _timeZone = 8;
    private bool showColon = true;
    private float updateInterval = 1f;
    private float timer = 0f;
    
    private void Start()
    {
        _accountInputField.text = PlayerPrefs.GetString(PWKey, "");
        _pwInputField.text = PlayerPrefs.GetString(AccountKey,"");
        
        _simpleButton.AddOnClickAction(()=>
        {
            _ = OnClickLogin();
        });
        
        _alarmButton.AddOnClickAction(()=>
        {
            _ = SaveAlarmSetting();
        });
        
        _timeZoneButton.AddOnClickAction(()=>
        {
            _ = SaveTimeZoneSetting();
        });
        
        _renewButton.AddOnClickAction(()=>
        {
            _ = InitMainPage();
        });
    }
    
    private void Update()
    {
        timer += Time.deltaTime;
        
        if (timer >= updateInterval)
        {
            timer = 0f;
            UpdateTimeDisplay();
        }
    }
    
    private void UpdateTimeDisplay()
    {
        DateTime utcNow = DateTime.UtcNow;
        DateTime utc8Time = utcNow.AddHours(_timeZone);
        
        showColon = !showColon;
        string timeString = utc8Time.ToString("HH") + (showColon ? ":" : " ") + utc8Time.ToString("mm");
        
        _timerText.text = timeString;
    }

    private async Task OnClickLogin()
    {
        if (_accountInputField.text == "" || _pwInputField.text == "")
        {
            LoginFailMessage();
            return;
        }
        
        var userAccount = _accountInputField.text;
        var pw = _pwInputField.text;
        await AuthenticationManager.Instance.SignInWithEmail(userAccount, pw);

        if (AuthenticationManager.Instance.GetFirebaseUser() != null)
        {
            Debug.Log("Login ok");

            PlayerPrefs.SetString(PWKey, userAccount);
            PlayerPrefs.SetString(AccountKey,pw);
            
            Debug.Log("1");

            await InitMainPage();
            Debug.Log("10");

            _popupDialog.StartPopupDialog("Login successful", onYesAction: () =>
            {
                _loginPage.AnimationLocalMovement(Vector2.zero, new Vector2(-2000, 0), 0.3f);
                _mainPage.gameObject.SetActive(true);
                _mainPage.AnimationLocalMovement(new Vector2(2000, 0), Vector2.zero, 0.3f);
            });
        }
        else
        {
            LoginFailMessage();
        }
    }

    private void LoginFailMessage() => _popupDialog.StartPopupDialog("Wrong username or password");

    private async Task InitMainPage()
    {
        Debug.Log("2");

        var result = await RealtimeDatabaseManager.Instance.GetValue(AlarmSettingPath);
        if (result != null)
        {
            var str = result.Value.ToString();
            str = str.Replace(',', ':');
            _alarmText.text = "Alarm Setting : " + str;
        }
        else
        {
            Debug.LogError("Get Alarm Setting Error");
            _alarmText.text = "Alarm Setting : -";
        }
        Debug.Log("3");

        var resultTimeZone = await RealtimeDatabaseManager.Instance.GetValue(TimeZonePath);
        if (resultTimeZone != null)
        {
            var str = resultTimeZone.Value.ToString();
            _timeZoneText.text = "Current Time Zone : " + str;
        }
        else
        {
            Debug.LogError("Get Time Zone Error");
            _timeZoneText.text = "Current Time Zone : -";
        }
        Debug.Log("4");

    }

    private async Task SaveAlarmSetting()
    {
        var alarmText = _alarmInputField.text;
        if (alarmText == "" || !alarmText.Contains(","))
        {
            _popupDialog.StartPopupDialog("Wrong alarm input. Please try again.");
            return;
        }
            
        await RealtimeDatabaseManager.Instance.SetValue(AlarmSettingPath, alarmText);
        _popupDialog.StartPopupDialog("Save Alarm Done");

        var str = alarmText;
        str = str.Replace(',', ':');
        _alarmText.text = "Alarm Setting : " + str;
    }
    
    private async Task SaveTimeZoneSetting()
    {
        var timeZoneText = _timeZoneInputField.text;
        if (timeZoneText == "")
        {
            _popupDialog.StartPopupDialog("Wrong time zone input. Please try again.");
            return;
        }
            
        await RealtimeDatabaseManager.Instance.SetValue(TimeZonePath, timeZoneText);
        _popupDialog.StartPopupDialog("Save Time zone Done");

        var str = timeZoneText;
        _timeZoneText.text = "Current Time Zone : " + str;

        if (int.TryParse(timeZoneText, out int t))
            _timeZone = t;
        else
            Debug.Log(timeZoneText);
    }
}
