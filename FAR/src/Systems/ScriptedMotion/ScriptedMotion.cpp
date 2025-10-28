#include "PCH/PCH.hpp"

#include "ScriptedMotion.hpp"

#include "Components/Transform.hpp"

#include "Engine/Engine.hpp"


namespace FAR
{
  void ScriptedMotion::Init()
  {
    renderResc = Engine::GetInstance()->GetResource<RenderResource>();

    Eigen::MatrixXd m;

    m.setRandom(10, 10);

    m(13) = 5.0;

    std::cout << "m is:\n" << m << std::endl;

    Eigen::Matrix<double, 10, 1> v;

    v.setRandom();

    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> system(m);

    Eigen::Matrix<double, 10, 1> x = system.solve(v);


    std::cout << "The solution is:\n" << x << std::endl;

    x.array().operator[](0) = 3;

    std::cout << "it is now:\n" << x << std::endl;
  }

  void ScriptedMotion::PreUpdate()
  {

  }

  void ScriptedMotion::Update()
  {
    std::vector<Entity> entities = Engine::GetInstance()->GetEntities<ScriptedMotionPath>();

    for (Entity e : entities)
    {
      ScriptedMotionPath& smp = Engine::GetInstance()->GetComponent<ScriptedMotionPath>(e);


      Eigen::MatrixXd mat = GetMatrixByNumCtrlPts(smp.controlPoints.size());

      if (smp.totalTime <= 0.0f)
       smp.totalTime = 0.1f;

      if (smp.isDirty)
      {


        ComputePath(smp, mat);
        //ComputeArcLengths(smp);
        smp.isDirty = false;
        smp.velCurveIntegral = VelocityCurveArea(smp, 1.0f);
        float test = VelocityCurveArea(smp, 0.5f);

        smp.arcLenTable.clear();
        smp.keyPoints.clear();

        smp.arcLenTable.push_back(std::make_pair(0.0f, 0.0f));

        ComputeArcLengthsAdaptive(smp, 0.0f, 1.0f, 0.0f);

        float totalLength = smp.arcLenTable.end()[-1].second;
        
        std::for_each(smp.arcLenTable.begin(), smp.arcLenTable.end(), [totalLength](std::pair<float, float>& p) {
          p.second /= totalLength;
          });
      }

      for (int i = 0; i < smp.controlPoints.size() - 1; i++)
      {
        renderResc->DrawRay(glm::vec4(smp.controlPoints[i], 1.0f), glm::vec4(smp.controlPoints[i + 1], 1.0f));
      }

      //for (int i = 0; i < smp.pathPoints.size() - 1; i++)
      //{
      //  renderResc->DrawRay(glm::vec4(smp.pathPoints[i], 1.0f), glm::vec4(smp.pathPoints[i + 1], 1.0f));
      //}

      for (int i = 0; i < smp.keyPoints.size() - 1; i++)
      {
        renderResc->DrawRay(glm::vec4(smp.keyPoints[i].pos, 1.0f), glm::vec4(smp.keyPoints[i + 1].pos, 1.0f));
      }

      //std::cout << "Cubic Spline Matrix:\n" << mat << std::endl;

    }

    entities = Engine::GetInstance()->GetEntities<ScriptedMotionPath, Transform>();
    for (Entity e : entities)
    {
      ScriptedMotionPath& smp = Engine::GetInstance()->GetComponent<ScriptedMotionPath>(e);
      Transform& transform = Engine::GetInstance()->GetComponent<Transform>(e);

      smp.t += Engine::GetInstance()->dt * (1.0f / smp.totalTime);
      while (smp.t > 1.0f)
        smp.t -= 1.0f;


      smp.currentSpeed = GetCurrentSpeed(smp);

      //smp.currentPos += GetVelocityAtTime(smp) * Engine::GetInstance()->dt;

      //float currentpos = smp.currentPos;
      //float currentpos = smp.t;
      
      //smp.currentSpeed = 

      float areaprog = VelocityCurveArea(smp, smp.t) / smp.velCurveIntegral;

      float currentpos = GetUfromArcLength(smp, areaprog);

      //float currentpos = GetUfromArcLength(smp);


      while (currentpos > 1.0f)
        currentpos -= 1.0f;

      // smp.pathPoints[i] = glm::vec3(x, y, z);

      //transform.position = glm::vec3(x, y, z);

      glm::vec3 currentPos = GetCurvePoint(smp, currentpos);
      glm::vec3 centerOfInterest = GetCurvePoint(smp, currentpos + 0.001f);


      glm::vec3 w = centerOfInterest - currentPos;

      glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

      glm::vec3 u = glm::cross(up, w);
      glm::vec3 v = glm::cross(w, u);

      w = glm::normalize(w);
      u = glm::normalize(u);
      v = glm::normalize(v);


      glm::mat4 translation = glm::mat4(
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(currentPos, 1.0f)
      );

      glm::mat4 rotation = glm::mat4(
        glm::vec4(u.x, u.y, u.z, 0.0f),
        glm::vec4(v.x, v.y, v.z, 0.0f),
        glm::vec4(w.x, w.y, w.z, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
      );

      //rotation = glm::transpose(rotation);

      transform.modelMatrix = translation * rotation;
      transform.matManuallyModified = true;

      //transform.modelMatrix = glm::transpose(transform.modelMatrix);

      //float pathPos = currentpos * (PATH_RESOLUTION - 1);
      //int index = (int)pathPos;
      //float frac = pathPos - (float)index;
      //if (index >= PATH_RESOLUTION - 1)
      //{
      //  index = PATH_RESOLUTION - 2;
      //  frac = 1.0f;
      //}

      ////lerp between 2 closest points
      //glm::vec3 pos = smp.pathPoints[index] * (1.0f - frac) + smp.pathPoints[index + 1] * frac;
      //transform.position = pos;

      ////rotation control
      //glm::vec3 dir = smp.pathPoints[index + 1] - smp.pathPoints[index];
      //dir = glm::normalize(dir); 
      //glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
      //glm::vec3 right = glm::normalize(glm::cross(up, dir));
      //up = glm::normalize(glm::cross(dir, right));
      //glm::quat transformQuat = glm::quat_cast(glm::mat3(right, up, dir));
      //transform.rotationQuaternion = Quat(transformQuat.w, transformQuat.x, transformQuat.y, transformQuat.z);
    }
  }



  void ScriptedMotion::PostUpdate()
  {

  }

  void ScriptedMotion::Exit()
  {

  }

  Eigen::MatrixXd ScriptedMotion::GetMatrixByNumCtrlPts(int numCtrlPts)
  {
    Eigen::MatrixXd matrix(numCtrlPts + 2, numCtrlPts + 2);

    //zero initialize
    matrix.setZero();

    for (int i = 0; i < numCtrlPts + 2; i++)
    {
      for (int j = 0; j < numCtrlPts; j++)
      {
        //matrix[i, j] = GetCubicSplineMatrixTerm(j, i);
        matrix(i, j) = GetCubicSplineMatrixTerm(j, i);
      }
    }

    matrix(2, numCtrlPts) = 1;
    matrix(2, numCtrlPts + 1) = 1;

    int k = numCtrlPts - 1;
    for (int i = 3; i < numCtrlPts + 2; i++)
    {
      int currentterm = k - (i - 3);
      matrix(i, numCtrlPts) = 0;
      matrix(i, numCtrlPts + 1) = 6 * currentterm;
    }

    return matrix;
  }

  double ScriptedMotion::GetCubicSplineMatrixTerm(int t, int termnum)
  {
    if (termnum == 0)
      return 1;

    if (termnum == 1)
      return t;

    if (termnum == 2)
      return t * t;

    if (termnum == 3)
      return t * t * t;


    int c = termnum - 3;
    int tMinusC = t - c;
    if (tMinusC < 0)
    {
      return 0;
    }

    return tMinusC * tMinusC * tMinusC;
  }

  void ScriptedMotion::ComputePath(ScriptedMotionPath& smp, Eigen::MatrixXd mat)
  {
    Eigen::VectorXd vecX(smp.controlPoints.size() + 2);
    Eigen::VectorXd vecY(smp.controlPoints.size() + 2);
    Eigen::VectorXd vecZ(smp.controlPoints.size() + 2);
    
    vecX.setZero();
    vecY.setZero();
    vecZ.setZero();

    smp.vecX.setZero();
    smp.vecY.setZero();
    smp.vecZ.setZero();

    for (int i = 0; i < smp.controlPoints.size(); i++)
    {
      vecX[i] = smp.controlPoints[i].x;
      vecY[i] = smp.controlPoints[i].y;
      vecZ[i] = smp.controlPoints[i].z;
    }

    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> system(mat.transpose());

    Eigen::VectorXd solvedX = system.solve(vecX);
    Eigen::VectorXd solvedY = system.solve(vecY);
    Eigen::VectorXd solvedZ = system.solve(vecZ);

    //smp.vecX = solvedX;
    //smp.vecY = solvedY;
    //smp.vecZ = solvedZ;

    smp.vecX = system.solve(vecX);
    smp.vecY = system.solve(vecY);
    smp.vecZ = system.solve(vecZ);

    float step = (float)(smp.controlPoints.size() - 1) / (PATH_RESOLUTION - 1);
    //float step = (float)(smp.controlPoints.size() - 1) / (5 - 1);

    float current = 0.0f;

    for (int i = 0; i < PATH_RESOLUTION; i++)
    //for (int i = 0; i < 5; i++)
    {
      float x = 0.0f;
      float y = 0.0f;
      float z = 0.0f;

      x += solvedX[0];
      x += solvedX[1] * current;
      x += solvedX[2] * current * current;
      x += solvedX[3] * current * current * current;

      y += solvedY[0];
      y += solvedY[1] * current;
      y += solvedY[2] * current * current;
      y += solvedY[3] * current * current * current;

      z += solvedZ[0];
      z += solvedZ[1] * current;
      z += solvedZ[2] * current * current;
      z += solvedZ[3] * current * current * current;

      for (int j = 4; j < smp.controlPoints.size() + 2; j++)
      {
        float t = current - (float)(j - 3);
        if (t < 0)
        {
          continue;
        }
        x += solvedX[j] * t * t * t;
        y += solvedY[j] * t * t * t;
        z += solvedZ[j] * t * t * t;
      }

      smp.pathPoints[i] = glm::vec3(x, y, z);

      current += step;
    }
  }

  float ScriptedMotion::VelocityCurveArea(ScriptedMotionPath& smp, float a)
  {
    //piecewise linear velocity curve

    if (a <= 0.0f)
      return 0.0f;

    if (smp.velocityKeys.size() == 0)
      return 0.0f;

    float area = (smp.velocityKeys[0].first * smp.velocityKeys[0].second) / 2.0f;

    for (int i = 1; i < smp.velocityKeys.size(); i++)
    {
      if (smp.velocityKeys[i].first <= a)
      {
        float xDiff = smp.velocityKeys[i].first - smp.velocityKeys[i - 1].first;
        float yDiff = smp.velocityKeys[i].second - smp.velocityKeys[i - 1].second;

        float rectarea = smp.velocityKeys[i - 1].second * xDiff;
        float triarea = (xDiff * yDiff) / 2;

        area += rectarea + triarea;
        //area += key.second * key.first;
      }
      else
      {
        float frac = (a - smp.velocityKeys[i - 1].first) / (smp.velocityKeys[i].first - smp.velocityKeys[i - 1].first);

        float interpX = a;
        float interpY = glm::mix(smp.velocityKeys[i - 1].second, smp.velocityKeys[i].second, frac);

        float xDiff = interpX - smp.velocityKeys[i - 1].first;
        float yDiff = interpY - smp.velocityKeys[i - 1].second;

        float rectarea = smp.velocityKeys[i - 1].second * xDiff;
        float triarea = (xDiff * yDiff) / 2;

        area += rectarea + triarea;
        break;
      }
    }
    return area;
  }

  //void ScriptedMotion::ComputeArcLengths(ScriptedMotionPath& smp)
  //{
    //smp.arcLengths[0] = 0.0f;
    //float totalLength = 0.0f;

    ////linear distance approximation
    //for (int i = 1; i < PATH_RESOLUTION; i++)
    //{
    //  float segmentLength = glm::length(smp.pathPoints[i] - smp.pathPoints[i - 1]);
    //  totalLength += segmentLength;
    //  smp.arcLengths[i] = totalLength;
    //}
    ////normalize
    //for (int i = 0; i < PATH_RESOLUTION; i++)
    //{
    //  smp.arcLengths[i] /= totalLength;
    //}
  //}

  void ScriptedMotion::ComputeArcLengthsAdaptive(ScriptedMotionPath& smp, float start, float end, float distance)
  {
    float epsilon = 0.000001f;

    //glm::vec3 startPoint = GetCurvePoint(smp, start);
    //glm::vec3 endPoint = GetCurvePoint(smp, end);

    //float middle = glm::mix(start, end, 0.5f);
    //glm::vec3 middlePoint = GetCurvePoint(smp, middle);

    //float startToEnd = glm::length(endPoint - startPoint);

    //float startToMiddle = glm::length(middlePoint - startPoint);
    //float throughMiddle = glm::length(middlePoint - startPoint) + glm::length(endPoint - middlePoint);

    start = 0.0f;
    end = 1.0f;

    int i = 0;

    ScriptedMotionPath::KeyPoint startkp = { .pos = GetCurvePoint(smp, start), .arcLenght = 0.0f, .u = 0.0f };
    smp.keyPoints.push_back(startkp);

    struct segment
    {
      float start;
      float end;

      glm::vec3 sp;
      glm::vec3 ep;
    };

    std::stack<segment> segments;

    segments.push(segment{ .start = 0.0f, .end = 1.0f, .sp = GetCurvePoint(smp, 0.0f), .ep = GetCurvePoint(smp, 1.0f)});

    float al = 0.0f;

    while (!segments.empty())
    {
      //glm::vec3 startPoint = GetCurvePoint(smp, start);
      segment current = segments.top(); segments.pop();

      //glm::vec3 startPoint = GetCurvePoint(smp, current.start);
      //glm::vec3 endPoint = GetCurvePoint(smp, current.end);

      const glm::vec3& startPoint = current.sp;
      const glm::vec3& endPoint = current.ep;


      float middle = glm::mix(current.start, current.end, 0.5f);
      glm::vec3 middlePoint = GetCurvePoint(smp, middle);

      float startToEnd = glm::length(endPoint - startPoint);
      float startToMiddle = glm::length(middlePoint - startPoint);
      float throughMiddle = glm::length(middlePoint - startPoint) + glm::length(endPoint - middlePoint);

      if (glm::length(startToEnd - throughMiddle) > epsilon)
      {
        segments.push(segment{ .start = middle, .end = current.end, .sp = middlePoint, .ep = endPoint});
        segments.push(segment{ .start = current.start, .end = middle, .sp = startPoint, .ep = middlePoint});
      }
      else
      {
        al += glm::length(startToMiddle);
        ScriptedMotionPath::KeyPoint kp = { .pos = middlePoint, .arcLenght = al, .u = middle };
        smp.keyPoints.push_back(kp);
        al += glm::length(endPoint - middlePoint);
        kp = { .pos = endPoint, .arcLenght = al, .u = current.end };
        //smp.keyPoints.push_back(kp);
      }
    }


    float totalLength = smp.keyPoints.back().arcLenght;

    //float totalLength = smp.keyPoints.end()[0].arcLenght;

    auto normalizeLength = [totalLength](ScriptedMotionPath::KeyPoint& kp) {kp.arcLenght /= totalLength; };
    std::for_each(smp.keyPoints.begin(), smp.keyPoints.end(), normalizeLength);

    smp.keyPoints.push_back(ScriptedMotionPath::KeyPoint{ .pos = GetCurvePoint(smp, end), .arcLenght = 1.0f, .u = 1.0f });
    //if (glm::abs(startToEnd - throughMiddle) > epsilon)
    //{
    //  ComputeArcLengthsAdaptive(smp, start, middle, distance);
    //  ComputeArcLengthsAdaptive(smp, middle, end, distance + startToMiddle);
    //}
    //else
    //{
    //  //store length
    //  smp.arcLenTable.push_back(std::make_pair(middle, distance + startToMiddle));
    //  //smp.arcLenTable.push_back(std::make_pair(end, distance + startToEnd));
    //}
  }

  float ScriptedMotion::GetUfromArcLength(ScriptedMotionPath& smp, float arclength)
  {
    //for (int i = 0; i < smp.arcLenTable.size() - 1; i++)
    //{
    //  if (arclength >= smp.arcLenTable[i].second && arclength <= smp.arcLenTable[i + 1].second)
    //  {
    //    float lengthDiff = smp.arcLenTable[i + 1].second - smp.arcLenTable[i].second;
    //    float frac = (arclength - smp.arcLenTable[i].second) / lengthDiff;
    //    return glm::mix(smp.arcLenTable[i].first, smp.arcLenTable[i + 1].first, frac);
    //  }
    //}

    //for (int i = 0; i < smp.keyPoints.size() - 1; i++)
    //{
    //  if (arclength >= smp.keyPoints[i].arcLenght && arclength <= smp.keyPoints[i + 1].arcLenght)
    //  {
    //    float lengthDiff = smp.keyPoints[i + 1].arcLenght - smp.keyPoints[i].arcLenght;
    //    float frac = (arclength - smp.keyPoints[i].arcLenght) / lengthDiff;
    //    return glm::mix(smp.keyPoints[i].u, smp.keyPoints[i + 1].u, frac);
    //  }
    //}

    //binary search

    if (smp.keyPoints.size() == 0)
      return 0;

    if (smp.keyPoints.size() == 1)
      return smp.keyPoints[0].u;

    int i = smp.keyPoints.size() / 2;
    int jumpsize = i / 2;

    bool found = false;
    int j = 0;

    while (!found)
    {
      j++;
      if (arclength >= smp.keyPoints[i].arcLenght && arclength <= smp.keyPoints[i + 1].arcLenght)
      {
        float segmentlength = smp.keyPoints[i + 1].arcLenght - smp.keyPoints[i].arcLenght;
        float frac = (arclength - smp.keyPoints[i].arcLenght) / segmentlength;

        //std::cout << j << std::endl;

        return glm::mix(smp.keyPoints[i].u, smp.keyPoints[i + 1].u, frac);
      }
      else if (arclength > smp.keyPoints[i + 1].arcLenght)
      {
        //i += (i / 2);
        //i = (smp.arcLenTable.size() + i) / 2;
        i += jumpsize;
      }
      else
      {
        //i /= 2;
        i -= jumpsize;
      }

      jumpsize /= 2;
      if (jumpsize == 0) jumpsize = 1;
    }
  }

  float ScriptedMotion::GetVelocityAtTime(ScriptedMotionPath& smp)
  {
    for (int i = 0; i < smp.velocityKeys.size() - 1; i++)
    {
      if (smp.t >= smp.velocityKeys[i].first && smp.t <= smp.velocityKeys[i + 1].first)
      {
        float timeDiff = smp.velocityKeys[i + 1].first - smp.velocityKeys[i].first;
        float frac = (smp.t - smp.velocityKeys[i].first) / timeDiff;
        //float velDiff = smp.velocityKeys[i + 1].second - smp.velocityKeys[i].second;
        

        return glm::mix(smp.velocityKeys[i].second, smp.velocityKeys[i + 1].second, frac);
        //return smp.velocityKeys[i].second + velDiff * frac;
      }
    }
    return 0.0f;
  }

  float ScriptedMotion::GetCurrentSpeed(ScriptedMotionPath& smp)
  {
    for (int i = 0; i < smp.velocityKeys.size() - 1; i++)
    {
      if (smp.t >= smp.velocityKeys[i].first && smp.t <= smp.velocityKeys[i + 1].first)
      {
        float timeDiff = smp.velocityKeys[i + 1].first - smp.velocityKeys[i].first;
        float frac = (smp.t - smp.velocityKeys[i].first) / timeDiff;
        return glm::mix(smp.velocityKeys[i].second, smp.velocityKeys[i + 1].second, frac);
      }
    }
    return 0.0f;
  }

    //int numcalls = 0;
  glm::vec3 ScriptedMotion::GetCurvePoint(ScriptedMotionPath& smp, float u)
  {
    //numcalls++;

    //smp.currentPos = currentpos;
    //Eigen::VectorXd vecX(smp.controlPoints.size() + 2);
    //Eigen::VectorXd vecY(smp.controlPoints.size() + 2);
    //Eigen::VectorXd vecZ(smp.controlPoints.size() + 2);

    //vecX.setZero();
    //vecY.setZero();
    //vecZ.setZero();

    //for (int i = 0; i < smp.controlPoints.size(); i++)
    //{
    //  vecX[i] = smp.controlPoints[i].x;
    //  vecY[i] = smp.controlPoints[i].y;
    //  vecZ[i] = smp.controlPoints[i].z;
    //}

    //Eigen::MatrixXd mat = GetMatrixByNumCtrlPts(smp.controlPoints.size());

    //Eigen::ColPivHouseholderQR<Eigen::MatrixXd> system(mat.transpose());

    //Eigen::VectorXd solvedX = system.solve(vecX);
    //Eigen::VectorXd solvedY = system.solve(vecY);
    //Eigen::VectorXd solvedZ = system.solve(vecZ);

    Eigen::VectorXd& solvedX = smp.vecX;
    Eigen::VectorXd& solvedY = smp.vecY;
    Eigen::VectorXd& solvedZ = smp.vecZ;

    //std::cout << solvedX << " vs " << solvedX1 << std::endl;
    //std::cout << solvedY << " vs " << solvedY1 << std::endl;
    //std::cout << solvedZ << " vs " << solvedZ1 << std::endl;

    //std::cout << ((solvedX[0] == solvedX1[0]) && (solvedX[1] == solvedX1[1]) && (solvedX[2] == solvedX1[2]) && (solvedX[3] == solvedX1[3])) << std::endl;
    //std::cout << ((solvedY[0] == solvedY1[0]) && (solvedY[1] == solvedY1[1]) && (solvedY[2] == solvedY1[2]) && (solvedY[3] == solvedY1[3])) << std::endl;
    //std::cout << ((solvedZ[0] == solvedZ1[0]) && (solvedZ[1] == solvedZ1[1]) && (solvedZ[2] == solvedZ1[2]) && (solvedZ[3] == solvedZ1[3])) << std::endl;


    float current = u * (smp.controlPoints.size() - 1);
    //float current = u;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    x += solvedX[0];
    x += solvedX[1] * current;
    x += solvedX[2] * current * current;
    x += solvedX[3] * current * current * current;

    y += solvedY[0];
    y += solvedY[1] * current;
    y += solvedY[2] * current * current;
    y += solvedY[3] * current * current * current;

    z += solvedZ[0];
    z += solvedZ[1] * current;
    z += solvedZ[2] * current * current;
    z += solvedZ[3] * current * current * current;

    for (int j = 4; j < smp.controlPoints.size() + 2; j++)
    {
      float t = current - (float)(j - 3);
      if (t < 0)
      {
        continue;
      }
      x += solvedX[j] * t * t * t;
      y += solvedY[j] * t * t * t;
      z += solvedZ[j] * t * t * t;
    }
    return glm::vec3(x, y, z);
  }
}